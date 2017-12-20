source /opt/yogurt/i.MX6-PD15.3.0/environment-setup-cortexa9hf-vfp-neon-phytec-linux-gnueabi
#!/bin/bash

# 使imx6的开发环境生效

make

$CC u_netlink.c -o u_netlink -lpthread
