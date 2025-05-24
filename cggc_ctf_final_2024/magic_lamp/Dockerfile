FROM docker.io/library/ubuntu:22.04@sha256:0e5e4a57c2499249aafc3b40fcd541e9a456aab7296681a3994d631587203f97
RUN DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y xinetd python3
RUN apt-get clean
RUN useradd -m user

ADD share/flag.txt /home/user/flag.txt
ADD share/server.py /home/user/server.py
ADD xinetd /etc/xinetd.d/user

RUN chown -R user:user /home/user/

USER user
WORKDIR /home/user/

CMD ["/usr/sbin/xinetd","-dontfork"]