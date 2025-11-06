FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y git
RUN apt-get install -y sudo
RUN apt-get install -y lsb-release
RUN apt-get install -y wget

WORKDIR /app/tegia-node
COPY . .

RUN chmod +x ./install.sh
RUN ./install.sh

CMD ["/bin/bash"]
