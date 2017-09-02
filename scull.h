#ifndef SCULL_H
#define SCULL_H

#define SCULL_CHARDEV_COUNT		2
#define SCULL_MINOR_NUMBER		666
#define SCULL0_MAJOR(dev)		MAJOR(dev)
#define SCULL0_MINOR(dev)		MINOR(dev+0)

#define SCULL_MAX_DATA			1000	/*bytes */
#define SCULL_MAX_DATA_COUNT	1000
/*scull_qset and scull_dev was taken from offisial scull code`
 *
 */
struct scull_qset {
	void **data;
	struct scull_qset *next;
};

struct scull_dev {
	struct scull_qset *data;	/* Pointer to first quantum set */
	int quantum;		/* the current quantum size */
	int qset;		/* the current array size */
	unsigned long size;	/* amount of data stored here */
	unsigned int access_key;	/* used by sculluid and scullpriv */
	struct mutex sem;	/* mutual exclusion semaphore     */
	struct cdev cdev;	/* Char device structure              */
};
#endif				/*SCULL_H */
