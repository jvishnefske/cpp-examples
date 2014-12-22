#include <memory>
#include <vector>
#include <iostream>
#include <sstream> //stringstream
class Elf{
    int id;
    //Elf(Elf &other): id(other.id){
    //}
    public:
    Elf(int _id):id(_id){
        std::cout << "new elf    (" << id << ")." << " \taddr " << this <<std::endl;
    }
    Elf (Elf const& other):id(other.id){
        std::cout << "copy of    (" << id << ") \taddr "<<this<<std::endl;}
    ~Elf(){
        std::cout << "killed elf (" << id << ")." << " \taddr " << 
           this << std::endl ;
    }
    std::string toString(){
        std::stringstream ss;
        ss << id;
        return ss.str();
    }
};
int main(int argc, char *argv[])
{
    typedef std::unique_ptr<Elf> ePtr;
    std::vector<std::unique_ptr<Elf>> plist;
    Elf *myelf = new Elf(400);
    plist.push_back(std::unique_ptr<Elf>(myelf));
    std::unique_ptr<Elf> fp(new Elf(55));
     
    //consider using boost::shared_pointer, unique_ptr, or boost::ptr_vector,  
    // unclear on the differences between the first two though.
    std::vector<Elf> elfbox;
    Elf e{312};
    std::cout << "creating two elves"<<std::endl;
    elfbox.push_back( Elf(2)); //requires public elf copy contructor for some reason.
    elfbox.push_back(e);
    //elfbox.push_back( Elf(3));
    //elfbox.push_back(Elf(199));
    int i=0;
    for (auto& e:plist){
        std::cout << "plist["<<i<<"] "<< e->toString() << std::endl;
        //std::cout<<e.toString()<<":";//std::endl;
        ++i;
    }
    std::cout <<std::endl;
    std::cout<<"poping two elves."<<std::endl;
    elfbox.pop_back();
    //elfbox.pop_back();
    plist.pop_back();
    std::cout << "leaving context." << std::endl;
    return 0;

}
