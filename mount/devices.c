/* devices.c */

/*
 * Copyright (C) 2017 Vitaliy Kopylov
 * http://latte-desktop.org/

Copyright (C) 2005 Jean-Baptiste jb_dul@yahoo.com

This program is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License 
as published by the Free Software Foundation; either 
version 2 of the License, or (at your option) any later 
version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <glib.h>
#include <fstab.h>
#include <mntent.h>
#include <sys/vfs.h>
#include <stdio.h>
#include "devices.h"

#define MTAB "/etc/mtab"
#define KB 1024
#define MB 1048576
#define GB 1073741824

static int g_strnox(char *str)
{
	int i = 0;
if (str == NULL) return -1;
	do {
		i++;
		if ((*str == '0') || (*str == '1') || (*str == '2') || (*str == '3') || (*str == '4') || (*str == '5') || (*str == '6') || (*str == '7') || (*str == '8') || (*str == '9'))
			return (i-1);
	} while (*str++);
	return (-1);
}


static char * g_strno(char *str)
{
char *s;
char *a;
char w;
s = g_strdup(str);
int i;
i = g_strnox(s);
if (i == -1){
	g_free(s);
	return (NULL);
}
w = s[i];
s[i]='\0';
a=g_strdup(s);
s[i]=w;
g_free(s);
	return (a);
}

static char * g_strchr(char *str, int ch)
{
	do {
		if (*str == ch)
			return (str);
	} while (*str++);
	return (NULL);
}

static int g_strlen(char *str)
{
	int i =0;
if (str == NULL) return -1;
	do {
		i++;
	} while (*str++);
	return (i-1);
}

static int g_strcmp(char *stra, char *strb)
{
int ia; int ib; int i;
if ((stra == NULL) || (strb == NULL)) return -1;
ia = g_strlen(stra);
ib = g_strlen(strb);
if (ia != ib) return -1;
	for(i=0;i < ia;i++)
	{
		if (stra[i] != strb[i]) return -1;
	}
	return 1;
}

/*-------------------- get_size_human_readable --------------------*/
/* return a string containing a size expressed in KB,MB or GB and the unit it is expressed in */
char * get_size_human_readable(float size)
{
	if (size < KB) return g_strdup_printf("%.1f B",size);
	if (size < MB) return g_strdup_printf("%.1f KB",size / KB);
	if (size < GB) return g_strdup_printf("%.1f MB",size / MB);
	return g_strdup_printf("%.1f GB", size / GB);
	
}
/*--------------------------------------------------------------------*/

/*-------------------------- mount_info_print --------------------------*/
void mount_info_print(t_mount_info * mount_info)
{
	if (mount_info != NULL)
	{
		printf("size : %g\n",mount_info->size);
		printf("used size : %g\n",mount_info->used);
		printf("available size : %g\n",mount_info->avail);
		printf("percentage used: %d\n",mount_info->percent);
		printf("file system type : %s\n",mount_info->type);
		printf("actual mount point : %s\n",mount_info->mounted_on);
	}
	return ;
}
/*-----------------------------------------------*/

/*------------------------- mount_info_new --------------------*/
t_mount_info * mount_info_new(float size,float used,float avail, unsigned int percent,char * type, char * mounted_on)
{
	if ( type != NULL && mounted_on != NULL && size != 0 )
	{
		t_mount_info * mount_info ;
		mount_info = g_new0(t_mount_info,1);
		mount_info->size = size;
		mount_info->used = used ;
		mount_info->avail = avail ;
		mount_info->percent = percent;
		mount_info->type = g_strdup(type);
		mount_info->mounted_on = g_strdup(mounted_on);
		return mount_info;
	}
	return NULL;
}
/*-----------------------------------------------------*/

/* ------------- mount_info_new_from_stat----------------*/
/* create a new struct t_mount_info from a struct statfs data */
t_mount_info * mount_info_new_from_stat(struct statfs * pstatfs, char * mnt_type,char * mnt_dir)
{
	if (pstatfs != NULL && mnt_type != NULL && mnt_dir !=NULL)
	{
		t_mount_info * mount_info ;
		float size ; // total size of device
		float used; // used size of device
		float avail; //Available size of device
		unsigned int percent ; //percentage used
		
		/*compute sizes in bytes */
		size = (float)pstatfs->f_bsize * (float)pstatfs->f_blocks ;
		used = (float)pstatfs->f_bsize * ((float)pstatfs->f_blocks - (float)pstatfs->f_bfree);
		avail = (float)pstatfs->f_bsize * (float)pstatfs->f_bavail;
		percent = ((float)pstatfs->f_blocks - (float)pstatfs->f_bavail) * 100 / (float)pstatfs->f_blocks;
		
		/*create new t_mount_info */
		mount_info = mount_info_new(size,used,avail,percent,mnt_type,mnt_dir) ;
		return mount_info ;
	}
	return NULL;
	
}
/*---------------------------------------------------------*/


/*-------- free a struct t_mount_info -------------------*/
void mount_info_free(t_mount_info * * mount_info)
{
	if (*mount_info != NULL)
	{
		g_free((*mount_info)->mounted_on);
		g_free((*mount_info)->type);
		g_free((*mount_info));
		*mount_info = NULL ;
	}
	return ;
}
/*------------------------------------------------------*/




/*------- print a t_disk struct using printf -------------*/
void disk_print(t_disk * pdisk)
{
	if (pdisk != NULL)
	{
		printf("disk : %s\n",pdisk->device);
		printf("mount point : %s\n", pdisk->mount_point);
		if (pdisk->mount_info != NULL)
			mount_info_print(pdisk->mount_info);	
		else printf("not mounted\n");
	}
	return;
}
/*----------------------------------------*/

/* create a new t_disk struct */
t_disk * disk_new( const char * dev, const char * mp)
{
	if ( dev != NULL && mp != NULL)
	{
		t_disk  * pdisk ;
		pdisk = g_new0(t_disk,1);
		pdisk->device = g_strdup(dev);
		pdisk->mount_point = g_strdup(mp) ;
		pdisk->mount_info = NULL ;
		return pdisk ;
	}
	return NULL ;
}
/* -------------------------------------*/

/* free a t_disk struct */
void disk_free(t_disk * * pdisk)
{
	if (*pdisk !=NULL)
	{
		g_free((*pdisk)->device);
		g_free((*pdisk)->mount_point);
		mount_info_free(&((*pdisk)->mount_info));
		g_free(*pdisk);
		*pdisk = NULL ;
	}
	return;
}
/* ----------------------------------------*/

/*------------ mount a t_disk ---------------*/
/* return exit status of the mount command*/
void disk_mount (t_disk * pdisk, char* mount_command)
{
	if (pdisk != NULL)
	{
		char * cmd ;
		char *a;
		char *b;
char *str = g_strdup(mount_command);
if (!g_strchr(str,'$')) {
		g_free(str);
		cmd = g_strconcat ("bash -c '", mount_command, " ", 
		                   pdisk->mount_point, "'", NULL);
} else {
int i;
i = g_strlen(str) - g_strlen(g_strchr(str,'$'));

str[i]='\0';
a=g_strdup(str);
str[i]='$';
b=g_strdup(g_strchr(str,'$')+1);
g_free(str);
		cmd = g_strconcat ("bash -c '", a, pdisk->mount_point, b, "'", NULL);
g_free(a); g_free(b);
}

/*		cmd = g_strconcat ("bash -c '", mount_command, " ", 
		                   pdisk->mount_point, NULL);
		                   
		if (on_mount_cmd != NULL)
			cmd = g_strconcat(cmd," && ",on_mount_cmd," ", pdisk->mount_point, " ' ", NULL);
		else
			cmd = g_strconcat(cmd," ' ",NULL);
			
		DBG("cmd :%s",cmd); */
		
/*		exec_cmd_silent(cmd,FALSE,FALSE);
*/
/*		ERR("exec-mo-cmd %s \n", cmd); */
		g_spawn_command_line_async(cmd, NULL);
		g_free(cmd);
	}
}
/*-------------------------------------------*/

/* --------------unmount a t_disk ----------------*/
/* return exit status of the mount command*/
void disk_umount (t_disk * pdisk, char* umount_command)
{
	if (pdisk != NULL)
	{
		char * cmd ;
		char *a;
		char *b;
char *str = g_strdup(umount_command);
if (!g_strchr(str,'$')) {
		g_free(str);
		cmd = g_strconcat ("bash -c '", umount_command, " ", 
		                   pdisk->mount_point, "'", NULL);
} else {
int i;
i = g_strlen(str) - g_strlen(g_strchr(str,'$'));

str[i]='\0';
a=g_strdup(str);
str[i]='$';
b=g_strdup(g_strchr(str,'$')+1);
g_free(str);
		cmd = g_strconcat ("bash -c '", a, pdisk->mount_point, b, "'", NULL);
g_free(a); g_free(b);
}


/*      #ifdef DEBUG
         g_printf("umount command: %s \n", umount_command);
      #endif */

		/* changed from pdisk->device to pdisk->mount_point */
/*		cmd = g_strconcat (umount_command, " ", pdisk->mount_point, NULL); */
		
		/*bresult = g_spawn_command_line_sync(cmd,&standard_output,&standard_error,&exit_status,&error);
		if (bresult)
		{
			if (exit_status) 
			{
				mount_info_free(&(pdisk->mount_info));
			}
			
			return exit_status ;
		}
		*/
/*		exec_cmd_silent (cmd, FALSE, FALSE);
*/
/*		ERR("exec-um-cmd %s \n", cmd);*/
		g_spawn_command_line_async(cmd, NULL);
		g_free(cmd);
	}
	//return -1 ;
}
/*------------------------------------------------*/

/*------------------------- disks_new ----------------*/
/* fill a GPtrArray with pointers on struct t_disk containing infos on devices and theoretical mount point. use setfsent() and getfsent(). */

GPtrArray * disks_new(gboolean include_NFSs)
{
	GPtrArray * pdisks ;
	t_disk * pdisk ;
	struct fstab * pfstab ;
	
	// open fstab
	if (setfsent() != 1)
		return NULL ; // on error return NULL
	
	pdisks = g_ptr_array_new();
	
	for(;;)
	{
		pfstab = getfsent() ; //read a line in fstab
		if (pfstab == NULL) break ; // on eof exit
		// if pfstab->fs_spec do not begins with /dev discard
		//if pfstab->fs_file == none discard
		if ( g_str_has_prefix(pfstab->fs_spec,"/dev/") && (g_str_has_prefix(pfstab->fs_file,"/") != 0) )
		{
			pdisk = disk_new(pfstab->fs_spec, pfstab->fs_file);
			g_ptr_array_add( pdisks , pdisk );

		}
	}
	endfsent(); //closes file
	return pdisks ;
}

/*--------------------- disks_free --------------------------*/
/* free a GPtrArray containing pointer on struct t_disk elements */
void disks_free(GPtrArray * * pdisks)
{
	if (*pdisks != NULL)
	{
		int i ;
		t_disk * pdisk ;
		for (i=0; i < (*pdisks)->len ; i++)
		{
			pdisk = (t_disk*)(g_ptr_array_index((*pdisks),i)) ;
			disk_free( &pdisk ) ;
		}
		g_ptr_array_free((*pdisks), TRUE);
		(*pdisks) = NULL ;
	}
}

/*----------------------- disks_print----------------------*/
/* print a GPtrArray containing pointer on struct t_disk elements */
void disks_print(GPtrArray * pdisks)
{
	int i ;
	for (i=0; i < pdisks->len ; i++)
	{
		disk_print( g_ptr_array_index(pdisks,i) ) ;
	}
	return ;

}

/*-------------------- disks_search -------------------------*/
/*CHANGE !!!!!!!!!!!!!!!!!! search on mount directory rather than device name */
/*return a pointer on FIRST struct t_disk containing char * device as device field 
if not found return NULL */
t_disk * disks_search(GPtrArray * pdisks, char * mount_point)
{
	int i ;
	for (i=0; i < pdisks->len ; i++)
	{
		if (g_ascii_strcasecmp(((t_disk*)g_ptr_array_index(pdisks,i))->mount_point,mount_point)==0)
			return g_ptr_array_index(pdisks,i);
	}
	return NULL;
}

/* -------- disks_free_mount_info ----------------*/
/* remove struct t_mount_info from a GPtrArray containing struct t_disk * elements */
void disks_free_mount_info(GPtrArray * pdisks)
{
	int i ;
	for (i=0; i < pdisks->len ; i++)
	{
		mount_info_free( &(((t_disk*)g_ptr_array_index(pdisks,i))->mount_info)) ;
	}
	return ;

}

char * rtdisk(GPtrArray * pdisks){
	int i;t_disk * pdisk;
	for(i=0;i < pdisks->len;i++)
	{
		pdisk = g_ptr_array_index(pdisks,i);
if ((g_strlen(pdisk->mount_point) == 1) && (g_strchr(pdisk->mount_point, '/') != NULL)) {// || (g_strcmp(pdisk->mount_point, "/boot")))) {
//g_print("lol %s \n", pdisk->mount_point);
	pdisk->is_hide = TRUE;
	return pdisk->device;
  }
  }
	return NULL;
}

char * rbdisk(GPtrArray * pdisks){
	int i;t_disk * pdisk;
	for(i=0;i < pdisks->len;i++)
	{
		pdisk = g_ptr_array_index(pdisks,i);
if (g_strcmp(pdisk->mount_point, "/boot")) {
//g_print("lol %s \n", pdisk->mount_point);
	pdisk->is_hide = TRUE;
	return pdisk->device;
  }
  }
	return NULL;
}
/* --------------- disks_refresh ----------------------*/
void procinf(GPtrArray * pdisks)
{
  FILE *partitions;
  char dev[256];
  char bdev[256];
  char *s; char *w; char *z; char *r; char *b;
  int i; int j; int ia = pdisks->len;
	t_disk * pdisk;
	t_disk * ddisk;
  r = g_strno(rtdisk(pdisks));
  b = g_strno(rbdisk(pdisks)); 
  partitions = fopen ("/proc/partitions", "r");
  if (partitions == NULL) {return;}
  while (getc (partitions) != '\n');    /* skip header */
  while (fscanf (partitions, "%s%s%s%s\n", dev, dev, bdev, dev) != EOF)
    { 
          s = g_strdup(dev);
          w = g_strdup(bdev);
          z = g_strconcat("/dev/", s, NULL);
if ((g_strlen(w) == 1) && ((g_strchr(w, '1') != NULL) || (g_strchr(w, '0') != NULL))) continue;
          pdisk = disk_new(z, s);
          pdisk->is_new = TRUE;
          g_ptr_array_add( pdisks , pdisk );
    }

  fclose (partitions);

	for(i=ia;i < pdisks->len;i++)
	{
		pdisk = g_ptr_array_index(pdisks,i);
		
		if (g_strcmp(g_strno(pdisk->device), r) == 1){
			pdisk->is_hide = TRUE;
		}
		if (g_strcmp(g_strno(pdisk->device), b) == 1){
			pdisk->is_hide = TRUE;
		}
		for(j=ia;j < pdisks->len;j++)
		{
			if (i == j) continue;
			ddisk = g_ptr_array_index(pdisks, j);
			if (g_strcmp(g_strno(pdisk->device), ddisk->device) == 1){
				ddisk->mount_point = NULL;
				ddisk->is_hide = TRUE;
//				g_print("found! %s %s\n", ddisk->device ,ddisk->mount_point);
			}
		}
		for(j=0;j < ia;j++)
		{
			if (i == j) continue;
			ddisk = g_ptr_array_index(pdisks, j);
			if (g_strcmp(pdisk->device, ddisk->device) == 1){
				pdisk->mount_point = NULL;
				pdisk->is_hide = TRUE;
//				g_print("found! %s %s\n", pdisk->device ,pdisk->mount_point);
			}
		}
	}
if (r) {g_free(r); }
if (b) {g_free(b);}
if (s) {g_free(s);}
if (w) {g_free(w);}
if (z) {g_free(z);}
}
/* refresh t_mount_info infos in a GPtrArray containing struct t_disk * elements */
void disks_refresh(GPtrArray * pdisks)
{
	/* using getmntent to get filesystems mount information */
	
	FILE * fmtab = NULL ; // file /etc/mtab
	struct mntent * pmntent = NULL; // struct for mnt info
	struct statfs * pstatfs = NULL ;
	
	t_mount_info * mount_info ;
	t_disk * pdisk ;
	
	/*remove t_mount_info for all devices */
	disks_free_mount_info(pdisks);
	
	/*allocate new struct statfs */
	pstatfs = g_new0(struct statfs,1);
	
	/*open file*/
	fmtab = setmntent(MTAB,"r"); 
	
	/* start looking for mounted devices */
	for(pmntent = getmntent(fmtab) ; pmntent!=NULL ; pmntent = getmntent(fmtab))
	{
		/*getstat on disk */
		if (statfs(pmntent->mnt_dir,pstatfs)==0 && (pstatfs->f_blocks != 0))
		{	/*if we got the stat and they block number is non zero */
			
			/* get pointer on disk from pdisks */
			/*CHANGED to reflect change in disk_search*/
			pdisk = disks_search(pdisks,pmntent->mnt_dir);
			if (pdisk == NULL) //if disk is not found in pdisks
			{
				/*create a new struct t_disk and add it to pdisks */
				/* test for "/dev/" and mnt_dir != none */
				if ( !g_str_has_prefix(pmntent->mnt_fsname,"/dev/") || (g_ascii_strcasecmp(pmntent->mnt_dir,"none") == 0) ) {
					if ( g_strrstr(pmntent->mnt_fsname,":") == NULL)
					 continue ; }
		
				pdisk = disk_new(pmntent->mnt_fsname,pmntent->mnt_dir);
				g_ptr_array_add(pdisks,pdisk);
			}
			
			/* create new t_mount_info */	
			mount_info = mount_info_new_from_stat(pstatfs, pmntent->mnt_type, pmntent->mnt_dir) ;
			/* add it to pdisk */
			pdisk->mount_info = mount_info ;
		
		}
	}
	
	g_free(pstatfs);
	endmntent(fmtab); //close file
	return ;
}
