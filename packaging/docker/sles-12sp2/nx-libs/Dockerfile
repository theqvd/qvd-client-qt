FROM registry.qindel.com:5000/qvd/sles12_min:latest

#### Configure packages repo ####
RUN zypper ar -G http://cpan-mirror.qindel.com/sles12-sp2-media/dvd1/ DVD1suse
RUN zypper ar -G http://cpan-mirror.qindel.com/sles12-sp2-media/sdk1/ SDK1suse
RUN zypper ar -G http://hulkbuster.qindel.com/sles/12SP2/QVD-4.1/ QVD
RUN zypper ref && zypper update -y

# Base build tools
RUN zypper in -y build git make bzip2 gzip perl-XML-Parser perl-LWP-Protocol-https awk python 
#hostname 
RUN git clone https://github.com/ArcticaProject/nx-libs.git
RUN mkdir -p /home/abuild && mkdir -p /home/abuild/rpmbuild/SOURCES /home/abuild/rpmbuild/RPMS /home/abuild/rpmbuild/SRPMS /home/abuild/rpmbuild/OTHER /home/abuild/rpmbuild/BUILD
RUN cd /nx-libs && git archive -o /home/abuild/rpmbuild/SOURCES/nx-libs-3.5.99.26.tar.gz --prefix=nx-libs-3.5.99.26/ 3.6.x
RUN cp --preserve=time /nx-libs/nx-libs.spec /home/abuild/rpmbuild/SOURCES
ADD files/qvd-pub.gpg /root
ADD files/qvd-secret.gpg /root
ADD files/sle12.2.conf /usr/lib/build/configs
ADD files/nx-libs.spec /home/abuild/rpmbuild/SOURCES/nx-libs.spec

RUN unlink /usr/lib/build/configs/default.conf && ln -s /usr/lib/build/configs/sle12.2.conf /usr/lib/build/configs/default.conf

COPY files/build.sh /
