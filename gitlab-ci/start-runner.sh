#/bin/sh
# to start a gitlab runner

docker run -d --name gitlab-runner-${USER} --restart always \
    -v /var/run/docker.sock:/var/run/docker.sock \
    -v $HOME/.local/gitlab-runner-config:/etc/gitlab-runner \
    gitlab/gitlab-runner:latest
