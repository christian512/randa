FROM ubuntu:latest

RUN apt-get update
#############
### RANDA ###
#############

# Setup dependencies 
RUN apt-get install build-essential cmake -y

###########
### GAP ###
###########

# Install dependencies
WORKDIR /root
RUN apt-get install libgmp-dev wget -y

# Install GAP
RUN wget https://github.com/gap-system/gap/releases/download/v4.12.2/gap-4.12.2.tar.gz
RUN tar -xf gap-4.12.2.tar.gz && rm gap-4.12.2.tar.gz
WORKDIR /root/gap-4.12.2
RUN ./configure
RUN make

# Build packages
WORKDIR /root/gap-4.12.2/pkg 
RUN ../bin/BuildPackages.sh

# Update PATH and add softlink to gap.sh
ENV PATH="${PATH}:/root/gap-4.12.2/bin"
RUN ln -s $(which gap.sh) /usr/bin/gap

############
### SAGE ###
############

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get install sagemath -y

##############
### PYTHON ###
##############

RUN apt install software-properties-common -y \
    && add-apt-repository ppa:deadsnakes/ppa \
    && apt update \
    && apt install python3.8 -y

############
### ENV ####
############
# Setup further things for the development environment
WORKDIR /root
RUN mkdir /root/user_data_randa/ \
    && ln -s /root/user_data_randa/.vscode-server /root/ \
    && ln -s /root/user_data_randa/.ssh /root/
RUN apt install -y openssh-client git htop 
