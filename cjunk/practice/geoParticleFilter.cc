#include <cmath>
#include <future>
class ParticleFilter{
    // run all models updates with std::async
    public:
    void run_pf(std::vector<LandmarkObs> observations, Map map_landmarks, 
                ParticleFilter::Model model, int num_particles, int num_iterations){
        std::vector<Particle> particles;
        std::vector<double> weights;
        std::vector<std::future<void>> futures;
        for(int i = 0; i < num_particles; i++){
            particles.push_back(Particle(map_landmarks));
        }
        for(int i = 0; i < num_iterations; i++){
            for(int j = 0; j < num_particles; j++){
                futures.push_back(std::async(std::launch::async, &Particle::update, &particles[j], model, observations, map_landmarks));
            }
            for(int j = 0; j < num_particles; j++){
                futures[j].get();
            }
            weights.clear();
            for(int j = 0; j < num_particles; j++){
                weights.push_back(particles[j].weight);
            }
            std::cout << "weights: " << weights << std::endl;
            std::cout << "normalized weights: " << ParticleFilter::normalize(weights) << std::endl;
            std::cout << "resampled particles: " << ParticleFilter::resample(particles, ParticleFilter::normalize(weights)) << std::endl;
        }
    }
    // normalize a vector of weights
    private:
    std::vector<double> normalize(std::vector<double> weights){
        double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
        std::vector<double> normalized_weights;
        for(int i = 0; i < weights.size(); i++){
            normalized_weights.push_back(weights[i] / sum);
        }
        return normalized_weights;
    }
    // resample particles according to normalized weights
    private:
    std::vector<Particle> resample(std::vector<Particle> particles, std::vector<double> weights){
        std::vector<Particle> resampled_particles;
        std::vector<double> cumulative_weights;
        cumulative_weights.push_back(weights[0]);
        for(int i = 1; i < weights.size(); i++){
            cumulative_weights.push_back(cumulative_weights[i - 1] + weights[i]);
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        for(int i = 0; i < particles.size(); i++){
            double random_number = dis(gen);
            int index = 0;
            while(random_number > cumulative_weights[index]){
                index++;
            }
            resampled_particles.push_back(particles[index]);
        }
        return resampled_particles;
    }
};

void test_particle_filter(){
    Map map_landmarks;
    map_landmarks.insert(std::make_pair(0, Landmark(0, 0)));
    map_landmarks.insert(std::make_pair(1, Landmark(1, 0)));
    map_landmarks.insert(std::make_pair(2, Landmark(0, 1)));
    map_landmarks.insert(std::make_pair(3, Landmark(1, 1)));
    std::vector<LandmarkObs> observations;
    observations.push_back(LandmarkObs(0, 0.0, 0.0));
    observations.push_back(LandmarkObs(1, 0.0, 0.0));
    observations.push_back(LandmarkObs(2, 0.0, 0.0));
    observations.push_back(LandmarkObs(3, 0.0, 0.0));
    ParticleFilter pf;
    pf.run_pf(observations, map_landmarks, ParticleFilter::Model(0.1, 0.2, 0.05, 0.05), 100, 10);
}

class Model{
    public:
    Model(double dt, double std_pos[], double std_landmark[]){
        this->dt = dt;
        this->std_pos[0] = std_pos[0];
        this->std_pos[1] = std_pos[1];
        this->std_landmark[0] = std_landmark[0];
        this->std_landmark[1] = std_landmark[1];
    }
    double dt;
    double std_pos[2];
    double std_landmark[2];
};
class LandmarkObs{
    public:
    LandmarkObs(int landmark_id, double dx, double dy){
        this->landmark_id = landmark_id;
        this->dx = dx;
        this->dy = dy;
    }
    int landmark_id;
    double dx;
    double dy;
};
class Particle{
    public:
    Particle(Map map_landmarks){
        this->x = 0.0;
        this->y = 0.0;
        this->theta = 0.0;
        this->weight = 1.0;
        this->map_landmarks = map_landmarks;
    }
    void update(Model model, std::vector<LandmarkObs> observations, Map map_landmarks){
        this->x += model.dt * std::cos(this->theta);
        this->y += model.dt * std::sin(this->theta);
        this->theta += model.dt * (0.2 * std::sin(10.0 * this->theta) + 0.1);
        this->weight = 1.0;
        for(int i = 0; i < observations.size(); i++){
            int landmark_id = observations[i].landmark_id;
            double landmark_x = map_landmarks.at(landmark_id).x_f;
            double landmark_y = map_landmarks.at(landmark_id).y_f;
            double distance = std::sqrt((this->x - landmark_x) * (this->x - landmark_x) + (this->y - landmark_y) * (this->y - landmark_y));
            double bearing = std::atan2(landmark_y - this->y, landmark_x - this->x) - this->theta;
            this->weight *= 1.0 / (std::sqrt(2.0 * M_PI * model.std_landmark[0] * model.std_landmark[1]) * std::exp(-(observations[i].dx * observations[i].dx + observations[i].dy * observations[i].dy) / (2.0 * model.std_landmark[0] * model.std_landmark[1])));
            this->weight *= 1.0 / (std::sqrt(2.0 * M_PI * model.std_pos[0] * model.std_pos[1]) * std::exp(-(distance * distance) / (2.0 * model.std_pos[0] * model.std_pos[1])));
            this->weight *= 1.0 / (std::sqrt(2.0 * M_PI * std::atan(model.std_pos[1] / model.std_pos[0])) * std::exp(-(bearing * bearing) / (2.0 * std::atan(model.std_pos[1] / model.std_pos[0]))));
        }
    }
    double x;
    double y;
    double theta;
    double weight;
    Map map_landmarks;
};

int main(){
    test_particle_filter();
    return 0;
}
