/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)lfs_balloc.c	7.6 (Berkeley) 11/30/89
 */

#include "param.h"
#include "systm.h"
#include "user.h"
#include "buf.h"
#include "proc.h"
#include "file.h"
#include "vnode.h"
#include "../ufs/inode.h"
#include "../ufs/fs.h"

/*
 * Bmap defines the structure of file system storage
 * by returning the physical block number on a device
 * given the inode and the logical block number in a file.
 */
bmap(ip, bn, bnp)
	register struct inode *ip;
	register daddr_t bn;
	daddr_t	*bnp;
{
	register struct fs *fs;
	register daddr_t nb;
	struct buf *bp;
	daddr_t *bap;
	int i, j, sh;
	int error;

	if (bn < 0)
		return (EFBIG);
	fs = ip->i_fs;

	/*
	 * The first NDADDR blocks are direct blocks
	 */
	if (bn < NDADDR) {
		nb = ip->i_db[bn];
		if (nb == 0) {
			*bnp = (daddr_t)-1;
			return (0);
		}
		*bnp = fsbtodb(fs, nb);
		return (0);
	}
	/*
	 * Determine the number of levels of indirection.
	 */
	sh = 1;
	bn -= NDADDR;
	for (j = NIADDR; j > 0; j--) {
		sh *= NINDIR(fs);
		if (bn < sh)
			break;
		bn -= sh;
	}
	if (j == 0)
		return (EFBIG);
	/*
	 * Fetch through the indirect blocks.
	 */
	nb = ip->i_ib[NIADDR - j];
	if (nb == 0) {
		*bnp = (daddr_t)-1;
		return (0);
	}
	for (; j <= NIADDR; j++) {
		if (error = bread(ip->i_devvp, fsbtodb(fs, nb),
		    (int)fs->fs_bsize, NOCRED, &bp)) {
			brelse(bp);
			return (error);
		}
		if ((bp->b_flags & B_CACHE) == 0)
			reassignbuf(bp, ITOV(ip));
		bap = bp->b_un.b_daddr;
		sh /= NINDIR(fs);
		i = (bn / sh) % NINDIR(fs);
		nb = bap[i];
		if (nb == 0) {
			*bnp = (daddr_t)-1;
			brelse(bp);
			return (0);
		}
		brelse(bp);
	}
	*bnp = fsbtodb(fs, nb);
	return (0);
}

/*
 * Balloc defines the structure of file system storage
 * by allocating the physical blocks on a device given
 * the inode and the logical block number in a file.
 */
balloc(ip, bn, size, bpp, flags)
	register struct inode *ip;
	register daddr_t bn;
	int size;
	struct buf **bpp;
	int flags;
{
	register struct fs *fs;
	register daddr_t nb;
	struct buf *bp, *nbp;
	struct vnode *vp = ITOV(ip);
	int osize, nsize, i, j, sh, error;
	daddr_t newb, lbn, *bap, pref, blkpref();

	*bpp = (struct buf *)0;
	if (bn < 0)
		return (EFBIG);
	fs = ip->i_fs;

	/*
	 * If the next write will extend the file into a new block,
	 * and the file is currently composed of a fragment
	 * this fragment has to be extended to be a full block.
	 */
	nb = lblkno(fs, ip->i_size);
	if (nb < NDADDR && nb < bn) {
		osize = blksize(fs, ip, nb);
		if (osize < fs->fs_bsize && osize > 0) {
			error = realloccg(ip, nb,
				blkpref(ip, nb, (int)nb, &ip->i_db[0]),
				osize, (int)fs->fs_bsize, &bp);
			if (error)
				return (error);
			ip->i_size = (nb + 1) * fs->fs_bsize;
			ip->i_db[nb] = dbtofsb(fs, bp->b_blkno);
			ip->i_flag |= IUPD|ICHG;
			if (flags & B_SYNC)
				bwrite(bp);
			else
				bawrite(bp);
		}
	}
	/*
	 * The first NDADDR blocks are direct blocks
	 */
	if (bn < NDADDR) {
		nb = ip->i_db[bn];
		if (nb != 0 && ip->i_size >= (bn + 1) * fs->fs_bsize) {
			error = bread(vp, bn, fs->fs_bsize, NOCRED, &bp);
			if (error) {
				brelse(bp);
				return (error);
			}
			*bpp = bp;
			return (0);
		}
		if (nb != 0) {
			/*
			 * Consider need to reallocate a fragment.
			 */
			osize = fragroundup(fs, blkoff(fs, ip->i_size));
			nsize = fragroundup(fs, size);
			if (nsize <= osize) {
				error = bread(vp, bn, osize, NOCRED, &bp);
				if (error) {
					brelse(bp);
					return (error);
				}
			} else {
				error = realloccg(ip, bn,
					blkpref(ip, bn, (int)bn, &ip->i_db[0]),
					osize, nsize, &bp);
				if (error)
					return (error);
			}
		} else {
			if (ip->i_size < (bn + 1) * fs->fs_bsize)
				nsize = fragroundup(fs, size);
			else
				nsize = fs->fs_bsize;
			error = alloc(ip, bn,
				blkpref(ip, bn, (int)bn, &ip->i_db[0]),
				nsize, &newb);
			if (error)
				return (error);
			bp = getblk(vp, bn, nsize);
			bp->b_blkno = fsbtodb(fs, newb);
			if (flags & B_CLRBUF)
				clrbuf(bp);
		}
		ip->i_db[bn] = dbtofsb(fs, bp->b_blkno);
		ip->i_flag |= IUPD|ICHG;
		*bpp = bp;
		return (0);
	}
	/*
	 * Determine the number of levels of indirection.
	 */
	pref = 0;
	sh = 1;
	lbn = bn;
	bn -= NDADDR;
	for (j = NIADDR; j > 0; j--) {
		sh *= NINDIR(fs);
		if (bn < sh)
			break;
		bn -= sh;
	}
	if (j == 0)
		return (EFBIG);
	/*
	 * Fetch the first indirect block allocating if necessary.
	 */
	nb = ip->i_ib[NIADDR - j];
	if (nb == 0) {
		pref = blkpref(ip, lbn, 0, (daddr_t *)0);
	        if (error = alloc(ip, lbn, pref, (int)fs->fs_bsize, &newb))
			return (error);
		nb = newb;
		bp = getblk(ip->i_devvp, fsbtodb(fs, nb), fs->fs_bsize);
		clrbuf(bp);
		if ((bp->b_flags & B_CACHE) == 0)
			reassignbuf(bp, vp);
		/*
		 * Write synchronously so that indirect blocks
		 * never point at garbage.
		 */
		if (error = bwrite(bp)) {
			blkfree(ip, nb, fs->fs_bsize);
			return (error);
		}
		ip->i_ib[NIADDR - j] = nb;
		ip->i_flag |= IUPD|ICHG;
	}
	/*
	 * Fetch through the indirect blocks, allocating as necessary.
	 */
	for (; ; j++) {
		error = bread(ip->i_devvp, fsbtodb(fs, nb),
		    (int)fs->fs_bsize, NOCRED, &bp);
		if (error) {
			brelse(bp);
			return (error);
		}
		if ((bp->b_flags & B_CACHE) == 0)
			reassignbuf(bp, vp);
		bap = bp->b_un.b_daddr;
		sh /= NINDIR(fs);
		i = (bn / sh) % NINDIR(fs);
		nb = bap[i];
		if (j == NIADDR)
			break;
		if (nb != 0) {
			brelse(bp);
			continue;
		}
		if (pref == 0)
			pref = blkpref(ip, lbn, 0, (daddr_t *)0);
		if (error = alloc(ip, lbn, pref, (int)fs->fs_bsize, &newb)) {
			brelse(bp);
			return (error);
		}
		nb = newb;
		nbp = getblk(ip->i_devvp, fsbtodb(fs, nb), fs->fs_bsize);
		clrbuf(nbp);
		if ((nbp->b_flags & B_CACHE) == 0)
			reassignbuf(nbp, vp);
		/*
		 * Write synchronously so that indirect blocks
		 * never point at garbage.
		 */
		if (error = bwrite(nbp)) {
			blkfree(ip, nb, fs->fs_bsize);
			brelse(bp);
			return (error);
		}
		bap[i] = nb;
		if (flags & B_SYNC)
			bwrite(bp);
		else
			bdwrite(bp);
	}
	/*
	 * Get the data block, allocating if necessary.
	 */
	if (nb == 0) {
		pref = blkpref(ip, lbn, i, &bap[0]);
		if (error = alloc(ip, lbn, pref, (int)fs->fs_bsize, &newb)) {
			brelse(bp);
			return (error);
		}
		nb = newb;
		nbp = getblk(vp, lbn, fs->fs_bsize);
		nbp->b_blkno = fsbtodb(fs, nb);
		if (flags & B_CLRBUF)
			clrbuf(nbp);
		bap[i] = nb;
		if (flags & B_SYNC)
			bwrite(bp);
		else
			bdwrite(bp);
		*bpp = nbp;
		return (0);
	}
	brelse(bp);
	nbp = getblk(vp, lbn, fs->fs_bsize);
	nbp->b_blkno = fsbtodb(fs, nb);
	if ((flags & B_CLRBUF) && (nbp->b_flags & (B_DONE|B_DELWRI)) == 0) {
		brelse(nbp);
		error = bread(vp, lbn, (int)fs->fs_bsize, NOCRED, &nbp);
		if (error) {
			brelse(nbp);
			return (error);
		}
	}
	*bpp = nbp;
	return (0);
}
