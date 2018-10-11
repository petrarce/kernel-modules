#ifndef SCULL_H
#define SCULL_H

#define SCULL_CHARDEV_COUNT		2
#define SCULL_MINOR_NUMBER		666
#define SCULL0_MAJOR(dev)		MAJOR(dev)
#define SCULL0_MINOR(dev)		MINOR(dev+0)

#define SCULL_MAX_DATA			1000	/*bytes */
#define SCULL_MAX_DATA_COUNT	1000
/*reverce_qset and reverce_dev was taken from offisial reverce code`
 *
 */
struct reverce_qset {
	void **data;
	struct reverce_qset *next;
};

struct reverce_dev {
	struct reverce_qset *data;	/* Pointer to first quantum set */
	int quantum;		/* the current quantum size */
	int qset;		/* the current array size */
	unsigned long size;	/* amount of data stored here */
	unsigned int access_key;	/* used by reverceuid and revercepriv */
	struct mutex sem;	/* mutual exclusion semaphore     */
	struct cdev cdev;	/* Char device structure              */
};
#endif				/*SCULL_H */
