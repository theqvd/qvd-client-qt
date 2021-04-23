FROM registry.suse.com/suse/sle15:15.2

# Prepare system
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1 installer-full-Media1
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1/Module-Basesystem/ Module-Basesystem
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1/Module-Development-Tools/ Module-Development-Tools
RUN zypper ar http://cpan-mirror.qindel.com/sles15-sp2/installer-full-Media1/Module-Python2/ Module-Python2
RUN zypper ar -G http://mirror.tspu.ru/opensuse/repositories/openSUSE:/Tools/SLE_15_SP2/openSUSE:Tools.repo
RUN zypper ref && zypper update -y

# Base build tools
RUN zypper in -y build git make bzip2 gzip perl-XML-Parser perl-LWP-Protocol-https awk python2 hostname rpm-sign
RUN git clone https://github.com/ArcticaProject/nx-libs.git
RUN mkdir -p /home/abuild && mkdir -p /home/abuild/rpmbuild/SOURCES /home/abuild/rpmbuild/RPMS /home/abuild/rpmbuild/SRPMS /home/abuild/rpmbuild/OTHER /home/abuild/rpmbuild/BUILD
RUN cd /nx-libs && git archive -o /home/abuild/rpmbuild/SOURCES/nx-libs-3.5.99.26.tar.gz --prefix=nx-libs-3.5.99.26/ 3.6.x
RUN cp --preserve=time /nx-libs/nx-libs.spec /home/abuild/rpmbuild/SOURCES
COPY files/build.sh /

