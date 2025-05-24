FROM docker.io/library/ubuntu:22.04@sha256:0e5e4a57c2499249aafc3b40fcd541e9a456aab7296681a3994d631587203f97
RUN DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y xinetd libc6-dev-i386 
RUN apt-get clean
RUN useradd -m oneshot

ADD share/flag.txt /home/oneshot/flag.txt
ADD share/chal /home/oneshot/chal
ADD share/run.sh /home/oneshot/run.sh
ADD xinetd /etc/xinetd.d/oneshot

RUN chown -R oneshot:oneshot /home/oneshot/

USER oneshot
WORKDIR /home/oneshot/

CMD ["/usr/sbin/xinetd","-dontfork"]