#include <linux/init.h>
#include <linux/module.h>
#include <linux/stat.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#include <net/netlink.h>
#include <net/sock.h>

#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/mman.h>
#include <linux/fs.h>

#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/pm.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include <linux/device.h>
#include <linux/dma-mapping.h>

#include <linux/io.h>

#include <linux/slab.h>
#include <linux/mman.h>
#include <linux/kthread.h>
#include <linux/dmaengine.h>
#include <linux/platform_data/dma-imx.h>
#include <linux/platform_data/dma-imx-sdma.h>

#include <linux/workqueue.h>

#define NETLINK_TEST (25)
#define NLMSG_PID    (100)

struct sock *nl_sk = NULL;

struct thread_data {
    int nr;
    pid_t pid;
    char * name;
};

static void netlink_send(int pid, uint8_t *message, int len)
{
    struct sk_buff *skb_1;
    struct nlmsghdr *nlh;

    printk ("len = %d \n", len);

    if (!message || !nl_sk) {
        return;
    }

    skb_1 = alloc_skb(NLMSG_SPACE(len), GFP_KERNEL);
    if (!skb_1) {
        printk(KERN_ERR "alloc_skb error!\n");
    }

    nlh = nlmsg_put(skb_1, 0, 0, 0, len, 0);
    NETLINK_CB(skb_1).portid = 0;
    NETLINK_CB(skb_1).dst_group = 0;
    memcpy(NLMSG_DATA(nlh), message, len);
	netlink_unicast(nl_sk, skb_1, pid, MSG_DONTWAIT);
}

static void netlink_input(struct sk_buff *__skb)
{
    struct sk_buff *skb;
//    char str[64];
    struct nlmsghdr *nlh;
//	struct completion cmpl;

    if (!__skb) {
        return;
    }

    skb = skb_get(__skb);
    if (skb->len < NLMSG_SPACE(0)) {
        return;
    }

    nlh = nlmsg_hdr(skb);
    if (NLMSG_PID != nlh->nlmsg_pid) {
        return ;
    }

    printk ("message: %s \n", (uint8_t *)NLMSG_DATA(nlh));
//    memset(str, 0, sizeof(str));
//    memcpy(str, NLMSG_DATA(nlh), sizeof(str));

//    init_completion(&cmpl);
//    wait_for_completion_timeout(&cmpl, 1 * HZ);

//    netlink_send(NLMSG_PID, NLMSG_DATA(nlh), nlh->nlmsg_len - NLMSG_SPACE(0));
    return;
}

static int netlinktest_work (void *data)
{
    int i = 10;
    printk ("netlink test start ...\n");

    while (i--) {
        msleep (3000);
        netlink_send(NLMSG_PID, "Hello kernel", strlen ("Hello kernel") + 1);
    }

    return 0;
}
static char *name = "netlinktest";

static __init int netlink_init(void)
{
    struct netlink_kernel_cfg nkc;

    ///////////////////////////////////////////////
    struct thread_data * thread;
    thread = kmalloc(sizeof(struct thread_data), GFP_KERNEL);
    if (!thread) {
        goto free_threads;
    }
    memset(thread, 0, sizeof(struct thread_data));
    thread->nr = 1;
    thread->name = name;
    kthread_run (netlinktest_work, thread, thread->name);
    ///////////////////////////////////////////////

    printk(KERN_WARNING "netlink init start!\n");

    //初始化netlink
    nkc.groups = 0;
    nkc.flags = 0;
    nkc.input = netlink_input;
    nkc.cb_mutex = NULL;
    nkc.bind = NULL;
    nkc.unbind = NULL;
    nkc.compare = NULL;
    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &nkc);
    if (!nl_sk) {
        printk(KERN_ERR "[netlink] create netlink socket error!\n");
        goto err;
    }

    printk(KERN_ALERT "netlink init success!\n");



    return 0;

err:
    netlink_kernel_release(nl_sk);

free_threads:
    kfree(thread);
    return -1;
}

static __exit void netlink_exit(void)
{
    netlink_kernel_release(nl_sk);
    printk(KERN_WARNING "netlink exit!\n");
}

module_init(netlink_init);
module_exit(netlink_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stone");
