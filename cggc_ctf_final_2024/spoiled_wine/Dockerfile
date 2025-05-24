FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update -y && \
    apt-get install -y --no-install-recommends \
        software-properties-common \
        ca-certificates \
        wget

# Install Wine
RUN dpkg --add-architecture i386 && \
    mkdir -pm755 /etc/apt/keyrings && \
    wget -O /etc/apt/keyrings/winehq-archive.key https://dl.winehq.org/wine-builds/winehq.key && \
    wget -nc -P /etc/apt/sources.list.d/ https://dl.winehq.org/wine-builds/ubuntu/dists/$(lsb_release -sc)/winehq-$(lsb_release -sc).sources && \
    apt-get update -y && \
    apt-get install -y --install-recommends winehq-stable

RUN apt-get install -y socat

RUN useradd -m user
ADD share/bof.exe /home/user/app.exe
ADD share/flag.txt /home/user/flag.txt
ADD run.sh /home/user/run.sh

RUN chown -R user:user /home/user/
USER user
WORKDIR /home/user/

# Just speed up wine execution afterwards
RUN wine nonexistent || true 

CMD ["/bin/bash", "/home/user/run.sh"]


