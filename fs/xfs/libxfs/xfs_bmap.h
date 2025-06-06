/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2000-2006 Silicon Graphics, Inc.
 * All Rights Reserved.
 */
#ifndef __XFS_BMAP_H__
#define	__XFS_BMAP_H__

struct getbmap;
struct xfs_bmbt_irec;
struct xfs_ifork;
struct xfs_inode;
struct xfs_mount;
struct xfs_trans;
struct xfs_alloc_arg;

/*
 * Argument structure for xfs_bmap_alloc.
 */
struct xfs_bmalloca {
	struct xfs_trans	*tp;	/* transaction pointer */
	struct xfs_inode	*ip;	/* incore inode pointer */
	struct xfs_bmbt_irec	prev;	/* extent before the new one */
	struct xfs_bmbt_irec	got;	/* extent after, or delayed */

	xfs_fileoff_t		offset;	/* offset in file filling in */
	xfs_extlen_t		length;	/* i/o length asked/allocated */
	xfs_fsblock_t		blkno;	/* starting block of new extent */

	struct xfs_btree_cur	*cur;	/* btree cursor */
	struct xfs_iext_cursor	icur;	/* incore extent cursor */
	int			nallocs;/* number of extents alloc'd */
	int			logflags;/* flags for transaction logging */

	xfs_extlen_t		total;	/* total blocks needed for xaction */
	xfs_extlen_t		minlen;	/* minimum allocation size (blocks) */
	xfs_extlen_t		minleft; /* amount must be left after alloc */
	bool			eof;	/* set if allocating past last extent */
	bool			wasdel;	/* replacing a delayed allocation */
	bool			aeof;	/* allocated space at eof */
	bool			conv;	/* overwriting unwritten extents */
	int			datatype;/* data type being allocated */
	uint32_t		flags;
};

#define	XFS_BMAP_MAX_NMAP	4

/*
 * Flags for xfs_bmapi_*
 */
#define XFS_BMAPI_ENTIRE	(1u << 0) /* return entire extent untrimmed */
#define XFS_BMAPI_METADATA	(1u << 1) /* mapping metadata not user data */
#define XFS_BMAPI_ATTRFORK	(1u << 2) /* use attribute fork not data */
#define XFS_BMAPI_PREALLOC	(1u << 3) /* preallocating unwritten space */
#define XFS_BMAPI_CONTIG	(1u << 4) /* must allocate only one extent */
/*
 * unwritten extent conversion - this needs write cache flushing and no additional
 * allocation alignments. When specified with XFS_BMAPI_PREALLOC it converts
 * from written to unwritten, otherwise convert from unwritten to written.
 */
#define XFS_BMAPI_CONVERT	(1u << 5)

/*
 * allocate zeroed extents - this requires all newly allocated user data extents
 * to be initialised to zero. It will be ignored if XFS_BMAPI_METADATA is set.
 * Use in conjunction with XFS_BMAPI_CONVERT to convert unwritten extents found
 * during the allocation range to zeroed written extents.
 */
#define XFS_BMAPI_ZERO		(1u << 6)

/*
 * Map the inode offset to the block given in ap->firstblock.  Primarily
 * used for reflink.  The range must be in a hole, and this flag cannot be
 * turned on with PREALLOC or CONVERT, and cannot be used on the attr fork.
 *
 * For bunmapi, this flag unmaps the range without adjusting quota, reducing
 * refcount, or freeing the blocks.
 */
#define XFS_BMAPI_REMAP		(1u << 7)

/* Map something in the CoW fork. */
#define XFS_BMAPI_COWFORK	(1u << 8)

/* Skip online discard of freed extents */
#define XFS_BMAPI_NODISCARD	(1u << 9)

/* Do not update the rmap btree.  Used for reconstructing bmbt from rmapbt. */
#define XFS_BMAPI_NORMAP	(1u << 10)

/* Try to align allocations to the extent size hint */
#define XFS_BMAPI_EXTSZALIGN	(1u << 11)

#define XFS_BMAPI_FLAGS \
	{ XFS_BMAPI_ENTIRE,	"ENTIRE" }, \
	{ XFS_BMAPI_METADATA,	"METADATA" }, \
	{ XFS_BMAPI_ATTRFORK,	"ATTRFORK" }, \
	{ XFS_BMAPI_PREALLOC,	"PREALLOC" }, \
	{ XFS_BMAPI_CONTIG,	"CONTIG" }, \
	{ XFS_BMAPI_CONVERT,	"CONVERT" }, \
	{ XFS_BMAPI_ZERO,	"ZERO" }, \
	{ XFS_BMAPI_REMAP,	"REMAP" }, \
	{ XFS_BMAPI_COWFORK,	"COWFORK" }, \
	{ XFS_BMAPI_NODISCARD,	"NODISCARD" }, \
	{ XFS_BMAPI_NORMAP,	"NORMAP" },\
	{ XFS_BMAPI_EXTSZALIGN,	"EXTSZALIGN" }


static inline int xfs_bmapi_aflag(int w)
{
	return (w == XFS_ATTR_FORK ? XFS_BMAPI_ATTRFORK :
	       (w == XFS_COW_FORK ? XFS_BMAPI_COWFORK : 0));
}

static inline int xfs_bmapi_whichfork(uint32_t bmapi_flags)
{
	if (bmapi_flags & XFS_BMAPI_COWFORK)
		return XFS_COW_FORK;
	else if (bmapi_flags & XFS_BMAPI_ATTRFORK)
		return XFS_ATTR_FORK;
	return XFS_DATA_FORK;
}

void xfs_bmap_alloc_account(struct xfs_bmalloca *ap);

/*
 * Special values for xfs_bmbt_irec_t br_startblock field.
 */
#define	DELAYSTARTBLOCK		((xfs_fsblock_t)-1LL)
#define	HOLESTARTBLOCK		((xfs_fsblock_t)-2LL)

/*
 * Flags for xfs_bmap_add_extent*.
 */
#define BMAP_LEFT_CONTIG	(1u << 0)
#define BMAP_RIGHT_CONTIG	(1u << 1)
#define BMAP_LEFT_FILLING	(1u << 2)
#define BMAP_RIGHT_FILLING	(1u << 3)
#define BMAP_LEFT_DELAY		(1u << 4)
#define BMAP_RIGHT_DELAY	(1u << 5)
#define BMAP_LEFT_VALID		(1u << 6)
#define BMAP_RIGHT_VALID	(1u << 7)
#define BMAP_ATTRFORK		(1u << 8)
#define BMAP_COWFORK		(1u << 9)

#define XFS_BMAP_EXT_FLAGS \
	{ BMAP_LEFT_CONTIG,	"LC" }, \
	{ BMAP_RIGHT_CONTIG,	"RC" }, \
	{ BMAP_LEFT_FILLING,	"LF" }, \
	{ BMAP_RIGHT_FILLING,	"RF" }, \
	{ BMAP_ATTRFORK,	"ATTR" }, \
	{ BMAP_COWFORK,		"COW" }

/* Return true if the extent is an allocated extent, written or not. */
static inline bool xfs_bmap_is_real_extent(const struct xfs_bmbt_irec *irec)
{
	return irec->br_startblock != HOLESTARTBLOCK &&
		irec->br_startblock != DELAYSTARTBLOCK &&
		!isnullstartblock(irec->br_startblock);
}

/*
 * Return true if the extent is a real, allocated extent, or false if it is  a
 * delayed allocation, and unwritten extent or a hole.
 */
static inline bool xfs_bmap_is_written_extent(const struct xfs_bmbt_irec *irec)
{
	return xfs_bmap_is_real_extent(irec) &&
	       irec->br_state != XFS_EXT_UNWRITTEN;
}

/*
 * Check the mapping for obviously garbage allocations that could trash the
 * filesystem immediately.
 */
#define xfs_valid_startblock(ip, startblock) \
	((startblock) != 0 || XFS_IS_REALTIME_INODE(ip))

int	xfs_bmap_longest_free_extent(struct xfs_perag *pag,
		struct xfs_trans *tp, xfs_extlen_t *blen);
void	xfs_trim_extent(struct xfs_bmbt_irec *irec, xfs_fileoff_t bno,
		xfs_filblks_t len);
unsigned int xfs_bmap_compute_attr_offset(struct xfs_mount *mp);
int	xfs_bmap_add_attrfork(struct xfs_trans *tp, struct xfs_inode *ip,
		int size, int rsvd);
void	xfs_bmap_local_to_extents_empty(struct xfs_trans *tp,
		struct xfs_inode *ip, int whichfork);
int xfs_bmap_local_to_extents(struct xfs_trans *tp, struct xfs_inode *ip,
		xfs_extlen_t total, int *logflagsp, int whichfork,
		void (*init_fn)(struct xfs_trans *tp, struct xfs_buf *bp,
				struct xfs_inode *ip, struct xfs_ifork *ifp,
				void *priv),
		void *priv);
void	xfs_bmap_compute_maxlevels(struct xfs_mount *mp, int whichfork);
int	xfs_bmap_first_unused(struct xfs_trans *tp, struct xfs_inode *ip,
		xfs_extlen_t len, xfs_fileoff_t *unused, int whichfork);
int	xfs_bmap_last_before(struct xfs_trans *tp, struct xfs_inode *ip,
		xfs_fileoff_t *last_block, int whichfork);
int	xfs_bmap_last_offset(struct xfs_inode *ip, xfs_fileoff_t *unused,
		int whichfork);
int	xfs_bmapi_read(struct xfs_inode *ip, xfs_fileoff_t bno,
		xfs_filblks_t len, struct xfs_bmbt_irec *mval,
		int *nmap, uint32_t flags);
int	xfs_bmapi_write(struct xfs_trans *tp, struct xfs_inode *ip,
		xfs_fileoff_t bno, xfs_filblks_t len, uint32_t flags,
		xfs_extlen_t total, struct xfs_bmbt_irec *mval, int *nmap);
int	xfs_bunmapi(struct xfs_trans *tp, struct xfs_inode *ip,
		xfs_fileoff_t bno, xfs_filblks_t len, uint32_t flags,
		xfs_extnum_t nexts, int *done);
void	xfs_bmap_del_extent_delay(struct xfs_inode *ip, int whichfork,
		struct xfs_iext_cursor *cur, struct xfs_bmbt_irec *got,
		struct xfs_bmbt_irec *del, uint32_t bflags);
void	xfs_bmap_del_extent_cow(struct xfs_inode *ip,
		struct xfs_iext_cursor *cur, struct xfs_bmbt_irec *got,
		struct xfs_bmbt_irec *del);
uint	xfs_default_attroffset(struct xfs_inode *ip);
int	xfs_bmap_collapse_extents(struct xfs_trans *tp, struct xfs_inode *ip,
		xfs_fileoff_t *next_fsb, xfs_fileoff_t offset_shift_fsb,
		bool *done);
int	xfs_bmap_can_insert_extents(struct xfs_inode *ip, xfs_fileoff_t off,
		xfs_fileoff_t shift);
int	xfs_bmap_insert_extents(struct xfs_trans *tp, struct xfs_inode *ip,
		xfs_fileoff_t *next_fsb, xfs_fileoff_t offset_shift_fsb,
		bool *done, xfs_fileoff_t stop_fsb);
int	xfs_bmap_split_extent(struct xfs_trans *tp, struct xfs_inode *ip,
		xfs_fileoff_t split_offset);
int	xfs_bmapi_convert_delalloc(struct xfs_inode *ip, int whichfork,
		xfs_off_t offset, struct iomap *iomap, unsigned int *seq);
int	xfs_bmap_add_extent_unwritten_real(struct xfs_trans *tp,
		struct xfs_inode *ip, int whichfork,
		struct xfs_iext_cursor *icur, struct xfs_btree_cur **curp,
		struct xfs_bmbt_irec *new, int *logflagsp);
xfs_extlen_t xfs_bmapi_minleft(struct xfs_trans *tp, struct xfs_inode *ip,
		int fork);
int	xfs_bmap_btalloc_low_space(struct xfs_bmalloca *ap,
		struct xfs_alloc_arg *args);
xfs_filblks_t xfs_bmap_worst_indlen(struct xfs_inode *ip, xfs_filblks_t len);

enum xfs_bmap_intent_type {
	XFS_BMAP_MAP = 1,
	XFS_BMAP_UNMAP,
};

#define XFS_BMAP_INTENT_STRINGS \
	{ XFS_BMAP_MAP,		"map" }, \
	{ XFS_BMAP_UNMAP,	"unmap" }

struct xfs_bmap_intent {
	struct list_head			bi_list;
	enum xfs_bmap_intent_type		bi_type;
	int					bi_whichfork;
	struct xfs_inode			*bi_owner;
	struct xfs_group			*bi_group;
	struct xfs_bmbt_irec			bi_bmap;
};

int	xfs_bmap_finish_one(struct xfs_trans *tp, struct xfs_bmap_intent *bi);
void	xfs_bmap_map_extent(struct xfs_trans *tp, struct xfs_inode *ip,
		int whichfork, struct xfs_bmbt_irec *imap);
void	xfs_bmap_unmap_extent(struct xfs_trans *tp, struct xfs_inode *ip,
		int whichfork, struct xfs_bmbt_irec *imap);

static inline uint32_t xfs_bmap_fork_to_state(int whichfork)
{
	switch (whichfork) {
	case XFS_ATTR_FORK:
		return BMAP_ATTRFORK;
	case XFS_COW_FORK:
		return BMAP_COWFORK;
	default:
		return 0;
	}
}

xfs_failaddr_t xfs_bmap_validate_extent_raw(struct xfs_mount *mp, bool rtfile,
		int whichfork, struct xfs_bmbt_irec *irec);
xfs_failaddr_t xfs_bmap_validate_extent(struct xfs_inode *ip, int whichfork,
		struct xfs_bmbt_irec *irec);
int xfs_bmap_complain_bad_rec(struct xfs_inode *ip, int whichfork,
		xfs_failaddr_t fa, const struct xfs_bmbt_irec *irec);

int	xfs_bmapi_remap(struct xfs_trans *tp, struct xfs_inode *ip,
		xfs_fileoff_t bno, xfs_filblks_t len, xfs_fsblock_t startblock,
		uint32_t flags);
int	xfs_bunmapi_range(struct xfs_trans **tpp, struct xfs_inode *ip,
		uint32_t flags, xfs_fileoff_t startoff, xfs_fileoff_t endoff);

extern struct kmem_cache	*xfs_bmap_intent_cache;

int __init xfs_bmap_intent_init_cache(void);
void xfs_bmap_intent_destroy_cache(void);

typedef int (*xfs_bmap_query_range_fn)(
	struct xfs_btree_cur	*cur,
	struct xfs_bmbt_irec	*rec,
	void			*priv);

int xfs_bmap_query_all(struct xfs_btree_cur *cur, xfs_bmap_query_range_fn fn,
		void *priv);

xfs_extlen_t	xfs_get_extsz_hint(struct xfs_inode *ip);
xfs_extlen_t	xfs_get_cowextsz_hint(struct xfs_inode *ip);

#endif	/* __XFS_BMAP_H__ */
