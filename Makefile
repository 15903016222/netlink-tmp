obj-m +=k_netlink.o

all:
	make -C /home/ws/project/phascan-II/linux SUBDIRS=$(PWD) modules
install:
	scp k_netlink.ko root@192.168.1.215:/tmp
	scp u_netlink    root@192.168.1.215:/tmp
clean:
	make -C /home/ws/project/phascan-II/linux SUBDIRS=$(PWD) clean 
	rm u_netlink
