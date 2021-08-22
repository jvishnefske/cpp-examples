#!/bin/sh

docker run --rm -it -v ${HOME}/.local/gitlab-runner-config:/etc/gitlab-runner gitlab/gitlab-runner register

