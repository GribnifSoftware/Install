#define NEW_AES
#include "aes.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#pragma warn -rpt
#include "install.rsi"
#pragma warn -w
#include "tos.h"
#include "lerrno.h"
#include "ierrno.h"

#define TXTBUFLEN	8000L
/*#define SCR_NAME	"DATA1\0\0\0\0\0\0"*/
#define SCR_NAME	"INSTALL.SCR"

#define _nflops *(int *)0x4a6
#define hdv_mediach  *((long *)0x47EL)
#define hdv_rw       *((long *)0x476L)

int line_num;
int dial=-1;
int nflops;
int button, skipping;
int txtlen, nvar;
char bigbuf[54*10], txtbuf[TXTBUFLEN], *txtptr;
OBJECT blank[] = { { -1, 1, 1, G_BOX, 0, 0, (long)(4<<4)|3 },
                   { 0, -1, -1, G_BOX, 32, 0, 0xFF1180 } };
extern long old_mediach, new_mediach, new_rw, old_rw;
int bad_media=-1;

#define ALRTL 140+1

char src_name[13]="Source", dest_name[13]="Destination",
  abortmsg[ALRTL]="Undo has been pressed. Abort the whole \
installation|or continue?][Abort|Continue]",
  diskfull[ALRTL]="||The destination disk is full!][Drats!]",
  notfound[ALRTL]="This file was not found on the disk!|Try a different disk?][Retry|Cancel]",
  exists[ALRTL]="The destination file already exists.|Do what?][Skip|Overwrite|Cancel]",
  readerr[ALRTL]="An error was encountered when reading this file!][Retry|Cancel]",
  writerr[ALRTL]="An error was encountered when writing this file!][Retry|Cancel]",
  baddata[ALRTL]="An error has occurred.|Skip the file or abort installation?][Skip|Abort]",
  folderr[ALRTL]="Folder %folder does not exist and|it could not be created][Retry|Cancel]",
  quitask[ALRTL]="Are you sure you want|to quit this program?][Quit|Stay]",
  swapmsg[ALRTL]="Please insert disk \"%diskname\" into drive %drive][Ok|Cancel]",
  runmsg[ALRTL]="Press any key to continue...",
  src_d[4] = "A:\\", dest_d[80] = "B:\\", sread[13]="Reading", swrite[13]="Writing",
  *dstd=&dest_d[0], *srcd=&src_d[0], mind[2]="A", maxd[2]="P", not_first,
  instdir[120], *instd=&instdir[0], tvar[5][120],
  *tv[5] = { &tvar[0][0], &tvar[1][0], &tvar[2][0], &tvar[3][0], &tvar[4][0] };
OBJECT *status, *disks;

typedef struct
{
  char lbl[8];
  int tree;
  char *str;
  int len;
} STRPTR;
STRPTR strptr[]={ { "SOURCE", -1, &src_name[0], 13 },
                { "DEST", -1, &dest_name[0], 13 },
                { "ABORTMSG", -1, &abortmsg[0], ALRTL-1 },
                { "DISKFULL", -1, &diskfull[0], ALRTL-1 },
                { "NOTFOUND", -1, &notfound[0], ALRTL-1 },
                { "EXISTS", -1, &exists[0], ALRTL-1 },
                { "READERR", -1, &readerr[0], ALRTL-1 },
                { "WRITERR", -1, &writerr[0], ALRTL-1 },
                { "BADDATA", -1, &baddata[0], ALRTL-1 },
                { "FOLDERR", -1, &folderr[0], ALRTL-1 },
                { "QUITASK", -1, &quitask[0], ALRTL-1 },
                { "SWAPMSG", -1, &swapmsg[0], ALRTL-1 },
                { "RUNMSG", -1, &runmsg[0], ALRTL-1 },
                { "SREAD", -1, &sread[0], 12 },
                { "SWRITE", -1, &swrite[0], 12 },
                { "MAXDRIVE", -1, &maxd[0], 1 },
                { "MINDRIVE", -1, &mind[0], 1 },
                { "DTITLE", DISKS, DTITLE, 25 },
                { "DINFO1", DISKS, DINFO1, 25 },
                { "DINFO2", DISKS, DINFO2, 25 },
                { "DSOURCE ", DISKS, DSOURCE, 25 },
                { "DDEST", DISKS, DDEST, 25 },
                { "DINSTALL", DISKS, DINSTALL, 10 },
                { "DQUIT", DISKS, DQUIT, 10 },
                { "SSTATUS ", STATUS, SSTATUS , 10 },
                { "SDRIVE", STATUS, SDRIVE, 10 },
                { "SNAME", STATUS, SNAME, 10 },
                { "SFOLDER", STATUS, SFOLDER, 10 },
                { "SFILE", STATUS, SFILE, 10 },
                { "SQUIT", STATUS, SQUIT, 10 },
                { "SUNDO", STATUS, SUNDO, 30 },
                { "SSTATMSG", STATUS, SSTATMSG, 10 },
                { "SDRIVMSG", STATUS, SDRIVMSG, 1 },
                { "SDISKNAM", STATUS, SDISKNAM, 10 },
                { "SFOLDNAM", STATUS, SFOLDNAM, 10 },
                { "SFILENAM", STATUS, SFILENAM, 10 },
                { 0L } };

typedef struct
{
  char lbl[8];
  char **val;
  int len;
} VARS;
VARS vars[]={ { "DISKNAME", &rs_tedinfo[4].te_ptext, 12 },
              { "DRIVE", &rs_tedinfo[24].te_ptext, 1 },
              { "SDRIVE", &srcd, 1 },
              { "SPATH", &srcd, -1 },
              { "DPATH", &dstd, -1 },
              { "DDRIVE", &dstd, 1 },
              { "FILENAME", &rs_tedinfo[8].te_ptext, 12 },
              { "INSTDIR", &instd, -1 },
              { "VAR1", &tv[0], -1 },
              { "VAR2", &tv[1], -1 },
              { "VAR3", &tv[2], -1 },
              { "VAR4", &tv[3], -1 },
              { "VAR5", &tv[4], -1 },
              { "FOLDER", &rs_tedinfo[6].te_ptext, 12 }, 0L };

long c_buflen, c_bufmax;
char *c_buf, *c_curbuf, *last_buf, skip_file;
int diskbuff[512];

#define CS_ODD      1
#define CS_FIRST    2
#define CS_LAST     4
#define CS_EMPTY    8
#define CS_IGNORE   16
#define CS_OVERWR   32
typedef struct
{
  char to[120];
  int time[2];
  long len;
  char att, state, noerr;
} copy_struct;

int var_alert( char *s );
int quit_alert( char *msg, int i );
void set_string( int tree, int num, char *s );
char *get_file(char *s);
char *get_fold(char *s);
void new_disk( char *name, char *s );
void echo( char *line );

void get_nflops(void)
{
  if( old_mediach )
  {
    hdv_mediach = old_mediach;
    hdv_rw = old_rw;
  }
  else
  {
    nflops = _nflops;
    old_mediach = hdv_mediach;
    hdv_mediach = (long)&new_mediach;
    old_rw = hdv_rw;
    hdv_rw = (long)&new_rw;
  }
}

void bee(void)
{
  graf_mouse( HOURGLASS, 0L );
}
void arrow(void)
{
  graf_mouse( ARROW, 0L );
}
void m_on(void)
{
  graf_mouse( M_ON, 0L );
}
void m_off(void)
{
  graf_mouse( M_OFF, 0L );
}

IOREC *io;
int TOS_error( long num )  /* -1: skip  0: noerr  other: alert */
{
  unsigned int l;

  if( num < 0L )
  {
    quit_alert( baddata, 2 );
    skip_file++;
    return(0);
  }
  if( io->ibufhd != (l=io->ibuftl) )
  {
    if( (l+=3) > io->ibufsiz ) l = 3;
    if( *((char *)io->ibuf+l)=='\03' || *((char *)io->ibuf+l-2)=='\x61' )
    {
      io->ibufhd = io->ibuftl;
      quit_alert( abortmsg, 1 );
    }
  }
  return(1);
}

void copy_init(void)
{
  c_buflen = ((long)Malloc(-1L) & 0xFFFFFFFEL) - 32000L;
  if( c_buflen<=1024L || (c_buf=(char *)Malloc(c_buflen)) == 0 )
  {
    c_buf = (char *)diskbuff;
    c_buflen = sizeof(diskbuff);
  }
  if( (long)c_buf&1 )
  {
    (long)c_buf &= 0xFFFFFFFEL;
    c_buflen--;
  }
  c_curbuf = c_buf;
  c_bufmax = c_buflen;
}
int pathend( char *ptr )
{
  register char *ch;

  if( (ch=strrchr(ptr,'\\')) == NULL ) return(0);
  return( ch - ptr + 1 );
}
void quitit( int stat )
{
  Supexec( (long (*)())get_nflops );
  wind_update( END_UPDATE );
  appl_exit();
  exit(stat);
}
int retry( long l, char *msg )
{
  TOS_error(0);
  if( l>=0 ) return(0);
  if( var_alert(msg) == 2 )
    if( var_alert(baddata) == 2 ) quitit(0);
    else
    {
      skip_file++;
      echo("");
      return(0);
    }
  echo("");
  return(1);
}
void dcreate( char *path )
{
  while( retry( Dcreate(path), folderr ) );
}
/********************************************************************/
int copy_confl( copy_struct *cs, int *hand, int *flg, int *conflict )
{
  /* flg:       name changed, tried to Fdelete the file
     conflict:  a conflict occurred */

  int i, ret=0;

  *conflict = *flg = 0;
  while( !*flg && !ret && cs->noerr && (*hand=Fsfirst( cs->to, 0x37 )) == 0 )
  {
    *conflict = 1;
    if( !(cs->state&CS_OVERWR) && quit_alert( exists, 3 ) == 1 )
    {
      skip_file++;
      ret=1;
    }
    else
    {
      cs->noerr = TOS_error( Fdelete(cs->to) );
      *flg = 1;
    }
  }
  return(ret);
}
/********************************************************************/
long dsetpath( char *path )
{
  char temp[120];
  long ret;
  DTA dma, *old;

  old = Fgetdta();
  Fsetdta(&dma);
  strcpy( temp, path );
  if( strlen(path)<3 ) temp[2] = '\\';
  strcpy( temp+3, "*.*" );
  if( (ret=Fsfirst( temp, 0x37 )) == 0 || ret==-33 )
      ret = Dsetpath( strlen(path)<3 ? "\\" : path+2 );
  Fsetdta(old);
  return(ret);
}
int check_dir( char *path )
                                /* make sure this directory does not already */
                                /* exist.   ret: 0=not there. 1=there */
{
  if( path[1]==':' ) Dsetdrv( (*path&0x5f) - 'A' );
  return !dsetpath( path ) || !Fsfirst( path, 0x37 );
}
void copy_buf( copy_struct *cs )
{
  long len2, len;
  char temp[120], *to, *ptr, temp2[2];
  int hand, flg, conflict;

  to = cs->to;
  set_string( STATUS, SSTATMSG, swrite );
  temp2[0] = *to;
  temp2[1] = 0;
  set_string( STATUS, SDRIVMSG, temp2 );
  set_string( STATUS, SDISKNAM, dest_name );
  set_string( STATUS, SFOLDNAM, get_fold(to) );
  set_string( STATUS, SFILENAM, get_file(to) );
  strcpy( ptr=temp, to );
  ptr = strchr(ptr,'\\');
  while( ptr && !skip_file )
  {
    strcpy( temp, to );
    if( (ptr=strchr(ptr+1,'\\')) != 0 )
    {
      *ptr = '\0';
      if( !check_dir( temp ) ) dcreate(temp);
    }
  }
  if( cs->state & CS_EMPTY ) return;
  if( cs->state & CS_FIRST )
  {
    if( copy_confl( cs, &hand, &flg, &conflict ) ) return;
    if( skip_file ) return;
    if( hand<0 && hand!=IEFILNF )
    {
      cs->noerr = TOS_error( (long)hand );
      if( skip_file ) return;
    }
    if( cs->noerr && (flg || hand < 0) )
    {
      bee();
      cs->noerr = TOS_error((long)(hand=Fcreate( to, 0 )));
      if( skip_file )
      {
        cs->noerr = 0;
        goto next;
      }
      conflict = flg;
      cs->state &= ~CS_FIRST;
    }
  }
  else
  {
    bee();
    if( (cs->noerr = TOS_error( (long)(hand =
        Fopen( to, 2 )) )) != 0 ) Fseek( 0L, hand, 2 );
    if( skip_file )
    {
      cs->noerr = 0;
      goto next;
    }
  }
  if( cs->noerr )
  {
    if( cs->len > 0L )
    {
      len2 = cs->len - (cs->state & CS_ODD);
      cs->noerr = TOS_error(0);
      if( skip_file )
      {
        cs->noerr = 0;
        goto next;
      }
      if( cs->noerr )
      {
        while( retry( len=Fwrite( hand, len2, (char *)cs+sizeof(copy_struct) ),
            writerr ) );
        if( skip_file ) cs->noerr = 0;
        else if( len != len2 )
        {
          var_alert( diskfull );
          cs->noerr = 0;
        }
      }
    }
next:
    Fclose(hand);
    if( !cs->noerr )
      if( !conflict && hand>0 ) Fdelete( to );
    if( cs->state & CS_LAST )
    {
      if( (hand = Fopen( to, 0 )) > 0 )
      {
        Fdatime( (DOSTIME *)cs->time, hand, 1 );
        Fclose(hand);
        Fattrib( to, 1, cs->att|0x20 );
      }
    }
  }
}

void blank_status(void)
{
  set_string( STATUS, SSTATMSG, "" );
  set_string( STATUS, SDRIVMSG, "" );
  set_string( STATUS, SDISKNAM, "" );
  set_string( STATUS, SFOLDNAM, "" );
  set_string( STATUS, SFILENAM, "" );
}

int copy_a_buffer(void)
{
  char noerr;
  register copy_struct *cs;

  if( !c_buf || c_buflen < 0L ) return(0);
  new_disk( dest_name, dest_d );
  cs = (copy_struct *)((long)c_buf);
  noerr = 1;
  while( (char *)cs < c_curbuf && noerr )
  {
    skip_file = 0;
    copy_buf( cs );
    if( !skip_file ) noerr = cs->noerr;
    cs = (copy_struct *)((long)cs +cs->len + sizeof(copy_struct));
  }
  noerr = ((copy_struct *)((long)c_buf))->noerr;
  c_curbuf = c_buf;
  c_buflen = c_bufmax;
  arrow();
  blank_status();
  return( !noerr );
}
/********************************************************************/
void reading( char *src )
{
  new_disk( src_name, src );
  set_string( STATUS, SSTATMSG, sread );
  set_string( STATUS, SDRIVMSG, src_d );
  set_string( STATUS, SDISKNAM, src_name );
  set_string( STATUS, SFOLDNAM, get_fold(src) );
  set_string( STATUS, SFILENAM, get_file(src) );
}
int copy_a_file( char *in, char *dest, int overwr )
{
  int hand=0, last=0, i, can;
  long len, l;
  register char noerr=1;
  register copy_struct *cs, cs2;
  char src[120];

  if( !not_first ) copy_init();
  cs = (copy_struct *)c_curbuf;
  cs->noerr = 1;
  skip_file = 0;
  strcpy( cs->to, dest[1]==':' ? "" : dest_d );
  strcat( cs->to, dest[0]=='\\' ? dest+1 : dest );
  if( !in )
  {
    cs->state = CS_EMPTY;
    if( *(cs->to+pathend(cs->to)) ) strcat( cs->to, "\\" );
    cs->len = 0L;
    c_buflen -= sizeof(copy_struct);
    c_curbuf += sizeof(copy_struct);
    last = 1;
    hand = -1;
    goto mkdir;
  }
  strcpy( src, in[1]==':' ? "" : src_d );
  strcat( src, in[0]=='\\' ? in+1 : in );
  if( !strcmp( src, cs->to ) ) return 0;
  reading(src);
  if( !not_first )
  {
    new_disk( src_name, in );
    not_first=1;
  }
  bee();
  while( retry( (long)(cs->att = Fattrib( src, 0, 0 )), notfound ) &&
      !skip_file );
  if( !skip_file )
    if( (hand=Fopen(src,0)) > 0 )
    {
      Fdatime( (DOSTIME *)cs->time, hand, 0 );
      cs->state = CS_FIRST;
      if( overwr ) cs->state |= CS_OVERWR;
      do
      {
        reading(src);
        len = c_buflen - sizeof(copy_struct);
        if( c_curbuf != (char *)cs )
        {
          memcpy( c_curbuf, cs, sizeof(copy_struct) );
          cs = (copy_struct *)c_curbuf;
        }
        c_curbuf += sizeof(copy_struct);
        if( (noerr = TOS_error(0)) != 0 && !skip_file )
        {
          while( retry( cs->len=Fread( hand, len, c_curbuf ), readerr ) );
          if( noerr && !skip_file )
          {
            if( cs->len == len )
            {
              l = Fseek( 0L, hand, 1 );
              if( (i = l==Fseek( 0L, hand, 2 )) == 0 ) Fseek( l, hand, 0 );
            }
            if( cs->len != len || i )
            {
              cs->state |= CS_LAST;
              last++;
              if( cs->len & 1 )
              {
                cs->state |= CS_ODD;
                cs->len++;
              }
            }
            if( noerr )
            {
              c_buflen = len - cs->len;
              c_curbuf += cs->len;
mkdir:        if( c_buflen < sizeof(copy_struct) || !last )/* changed from <= */
                if( (noerr = !copy_a_buffer()) != 0 )
                {
                  new_disk( src_name, src_d );
                  bee();
                }
            }
          }
        }
      } while( !skip_file && !last && noerr );
      if( hand>0 ) Fclose(hand);
    }
    else noerr=0;
  blank_status();
  return( !noerr );
}
/**********************************************************************/
char *get_line(void)
{
  static char *ptr, open=0;
  static long len=0L;
  static int hex;
  int quit=0, strt=1, hex_flg=0, ign_eol=0, com=0, hand;
  int from_hex( char *ptr );
  char *ret, *optr;

  if(!open)
  {
    if( (hand = Fopen( SCR_NAME, 0 )) < 0 )
    {
      sprintf( instdir, "[1][|Could not open|%s!][Goodbye]", SCR_NAME );
      form_alert( 1, instdir );
      quitit(hand);
    }
    if( (len = Fseek( 0L, hand, 2 )) <= 0 ) goto rderr;
    if( (ptr=Malloc(len)) == 0 )
    {
      form_alert( 1, "[1][You do not have enough|free memory to run|\
this program][Ok]" );
      quitit(-1);
    }
    Fseek( 0L, hand, 0 );
    if( Fread( hand, len, ptr ) != len )
    {
rderr:sprintf( instdir, "[1][|Error reading|%s!][Bye]", SCR_NAME );
      form_alert( 1, instdir );
      quitit(-1);
    }
    Fclose(hand);
    open++;
  }
  if( !len ) return(0L);
  ret = optr = ptr;
  while( !quit && len )
  {
    if( *ptr=='\r' )
    {
      line_num++;
      if( !com && !ign_eol )
      {
        *optr = '\0';
        quit++;
      }
      strt = 1;
      com=ign_eol=0;
    }
    else if( strt && *ptr!='\n' )
    {
      com = *ptr==';';
      strt=0;
    }
    if( !com && !strt )
      if( hex_flg )
      {
        hex = hex*16 + from_hex(ptr);
        if( hex_flg == 2 )
        {
          *optr++ = hex;
          hex_flg = 0;
        }
        else hex_flg++;
      }
      else if( *ptr=='^' && !ign_eol ) ign_eol++;
      else if( *ptr=='~' )
      {
        hex=0;
        hex_flg++;
      }
      else if( *ptr != '\n' ) *optr++ = *ptr;
    ptr++;
    len--;
  }
  if( !len ) *optr = '\0';
  return(ret);
}
/**********/
int from_hex( char *ptr )
{
  if( *ptr > 0x60 ) *ptr -= 0x20;             /* convert a-f to A-F */
  return( *ptr - (*ptr >= 'A' ? 0x37 : '0') );   /* convert hex digit to dec */
}

void set_string( int tree, int num, char *s )
{
  OBJECT *ob;

  ob = rs_trindex[tree]+num;
  if( ob->ob_type == G_BUTTON || ob->ob_type == G_STRING )
      ob->ob_spec.free_string = s;
  else ob->ob_spec.tedinfo->te_ptext = s;
  if( dial == tree ) objc_draw( rs_trindex[tree], num, 8, 0, 0, 32764, 32764 );
}

int is_sep( int ch )
{
  return( ch==' ' || ch=='\t' );
}

int strxcmp( char *a, char *b, int l )
{
  register char c1, c2;

  c1 = *a++;
  c2 = *b++;
  while( l-- && (c1 || c2) )
  {
    if( c2>='a' && c2<='z' ) c2 &= 0x5f;
    else if( is_sep(c2) ) return( l & c1 );
    if( c1 != c2 ) return(1);
    c1 = *a++;
    c2 = *b++;
  }
  return(0);
}

void insert_vars( char *b, char *s )
{
  VARS *v;
  char *p;
  int l, i;

top:
  while( *s )
    if( *s == '%' )
    {
      for( v=vars; v->lbl[0]; v++ )
      {
        l = v->lbl[7] ? 8 : strlen(v->lbl);
        if( !strxcmp( v->lbl, s+1, l ) )
        {
          i = v->len;
          for( p=*(v->val); i-- && *p; )
            *b++ = *p++;
          s += l+1;
          goto top;
        }
      }
      if( !v->lbl[0] ) *b++ = *s++;
    }
    else *b++ = *s++;
  *b++ = '\0';
}

int do_form( int ind )
{
  int i;

  arrow();
  (rs_trindex[ind]+(i=form_do( rs_trindex[ind], 0 ))) ->
      ob_state &= ~SELECTED;
  return(i);
}

void askdrive( char *line )
{
  int i, m;

  for( i=0; i<16; i++ )
    disks[i+DDESTA].ob_flags &= ~HIDETREE;
  for( i=0; i<(mind[0]&=0x5f)-'A'; i++ )
    disks[i+DDESTA].ob_flags |= HIDETREE;
  for( i=(maxd[0]&0x5f)-'A'+1; i<16; i++ )
    disks[i+DDESTA].ob_flags |= HIDETREE;
  if( dest_d[0] < mind[0] ) dest_d[0] = mind[0];
  for( m=Drvmap(), i=16; --i >= 0; )
  {
    if( !(m&(1<<i)) ) disks[i+DDESTA].ob_state |= DISABLED;
    if( i==dest_d[0]-'A' ) disks[i+DDESTA].ob_state |= SELECTED;
    else disks[i+DDESTA].ob_state &= ~SELECTED;
  }
  disks[DSRCA+src_d[0]-'A'].ob_state |= SELECTED;
  disks[DSRCA+1-src_d[0]-'A'].ob_state &= ~SELECTED;
  if( nflops==1 ) disks[DDESTA+1].ob_state = disks[DSRCA+1].ob_state |=
      DISABLED;
  objc_draw( disks, 0, 8, 0, 0, 32767, 32767 );
  if( do_form( DISKS ) == DQUIT ) quitit(0);
  src_d[0] = disks[DSRCA].ob_state&SELECTED ? 'A' : 'B';
  strcpy( src_d+1, ":\\" );
  for( i=17; --i >= 0; )
    if( disks[i+DDESTA].ob_state&SELECTED ) dest_d[0] = 'A'+i;
  strcpy( dest_d+1, ":\\" );
  line++;
}

void setdest( char *line )
{
  dest_d[0] = line[0]&0x5F;
  if( !(Drvmap() & (1<<(dest_d[0]-'A'))) ) dest_d[0] = 'A';
}

void set_buttons( char *line )
{
  int i;
  char *p;

  for( i=0, p=line; i<3; )
    if( !*line )
    {
      if( *p ) goto ok;
      status[SBUTTON1 + i++].ob_flags |= HIDETREE;
    }
    else if( *line == '|' )
    {
      if( p != line )
      {
ok:     strncpy( status[i+SBUTTON1].ob_spec.free_string, p, line-p );
        status[i+SBUTTON1].ob_spec.free_string[line-p] = '\0';
        status[SBUTTON1 + i++].ob_flags &= ~HIDETREE;
        if( *line ) line++;
      }
      else line++;
      p = line;
    }
    else line++;
  if( dial==STATUS ) objc_draw( status, SBUTTON1-1, 8, 0, 0, 32767, 32767 );
}

void ask( char *line )
{
  int b;
  char buf[140];

  for(;;)
  {
    set_buttons(line);
    objc_draw( status, SBUTTON1-1, 8, 0, 0, 32767, 32767 );
    if( (b=do_form( STATUS )) == SQUIT )
    {
      sprintf( buf, "[2][%s", quitask );
      if( form_alert( 1, buf ) == 1 ) quitit(0);
    }
    else break;
  }
  set_buttons( "" );
  skipping=1;
  button = b;
}

void fatal( char *s, int val );

int var_alert( char *s )
{
  char buf[ALRTL], *p, *q;
  int b0, b1;

  insert_vars( buf, s );
  if( (p=strchr(buf,']')) != 0 )
  {
    *p = '\0';
    echo(buf);
    status[SQUIT].ob_flags |= HIDETREE;
    if( (q=strchr(p += 2,']')) == 0 ) goto bad;
    *q = '\0';
    b0 = button;
    ask(p);
    b1 = button;
    button = b0;
    skipping=0;
    status[SQUIT].ob_flags &= ~HIDETREE;
    return( b1-SBUTTON1+1 );
  }
  else
bad:   fatal( "[1][Error in alert|string format][Ok]", 0 );
  return(0);
}

void askend( char *line )
{
  button=skipping=0;
  line++;
}

void ifend( char *line )
{
  skipping=0;
  line++;
}

void do_button( int i )
{
  skipping = i!=button;
}

void button1( char *line )
{
  do_button(SBUTTON1);
  line++;
}

void button2( char *line )
{
  do_button(SBUTTON1+1);
  line++;
}

void button3( char *line )
{
  do_button(SBUTTON1+2);
  line++;
}

void disp_string( int r, char *p )
{
  char *q, buf[60];
  int w;

  insert_vars( buf, p );
  if( (w = (54-strlen(buf))>>1) < 0 ) w=0;
  strncpy( disp[r]+w, buf, 54-w );
  while(w)
    disp[r][--w] = ' ';
  objc_draw( status, SSTRING1+r, 8, 0, 0, 32767, 32767 );
}

void echo( char *line )
{
  int r=0;
  char *p;

  p = line;
  while( *line && r<10 )
    if( *line=='|' )
    {
      *line++ = '\0';
      disp_string( r++, p );
      p = line;
    }
    else line++;
  if( r<10 ) disp_string( r++, p );
  while( r<10 ) disp_string( r++, "" );
}

char *get_fold(char *s)
{
  char *p;
  static char buf[13];

  if( (p=strrchr(s,'\\')) != 0 )
    while( p!=s )
      if( *--p == '\\' )
      {
        strncpy( buf, p+1, 12 );
        if( (p=strchr(buf,'\\')) != 0 ) *p = '\0';
        return(buf);
      }
  return("");
}

char *get_file(char *s)
{
  char *p;

  return( (p=strrchr(s,'\\')) != 0 ? p+1 : s );
}

void flush( char *line )
{
  copy_a_buffer();
  line++;
}

#define Hz_200 *(long *)0x4ba
void pause( char *line )
{
  long stack, l;

  m_off();
  stack = Super(0L);
  l = Hz_200 + atol(line)*200L;
  while( Hz_200<l );
  Super( (void *)stack );
  m_on();
  line++;
}

void more( char *line )
{
  m_off();
  Bconin(2);
  m_on();
  line++;
}

void doexit( char *line )
{
  quitit(0);
  line++;
}

int quit_alert( char *msg, int i )
{
  int r;

  while( (r=var_alert(msg)) == i )
    if( var_alert(quitask) == 1 ) quitit(0);
  echo("");
  return(r);
}

void set_name( char *line, char *s )
{
  status[SDISKNAM].ob_spec.tedinfo->te_ptext = line;
  status[SDRIVMSG].ob_spec.tedinfo->te_ptext = s;
}

void new_disk( char *line, char *s )
{
  char foo[] = "x:\\*.*";
  char *o;
  static char anam[15], bnam[15];

  set_name( line, s );
  if( *s < 'C' )
  {
    o = *s=='A' ? anam : bnam;
    if( strcmp( o, line ) )
    {
      strcpy( o, line );
      quit_alert( swapmsg, 2 );
      bad_media = ((foo[0] = *s)&0x5f) - 'A';
      Fsfirst( foo, 0x37 );
    }
  }
}

char *next_item( char *ptr )
{
  while( ptr && !is_sep( *ptr ) ) ptr++;
  *ptr++ = '\0';
  while( is_sep(*ptr) ) ptr++;
  return ptr;
}

void swapsrc( char *line )
{
  char *ptr=line;

  if( (*ptr&=0x5f) < 'C' )
  {
    line = next_item(line);
    strcpy( src_name, line );
    new_disk( src_name, ptr );
  }
}

void untilex( char *line )
{
  static char temp[11];

  strcpy( temp, next_item(line) );
  set_name( temp, line );
  while( retry( Fsfirst( line, 0x37 ), swapmsg ) );
}

void ifnex( char *line )
{
  if( !Fsfirst( line, 0x37 ) ) skipping++;
}

void ifex( char *line )
{
  if( Fsfirst( line, 0x37 ) ) skipping++;
}

void swapdest( char *line )
{
  flush(line);
  strcpy( dest_name, line );
}

void mkdir( char *line )
{
  copy_a_file( NULL, line, 0 );
}

void bell( char *line )
{
  Bconout(2,7);
  line++;
}

void copy( char *line )
{
  copy_a_file( line, next_item(line), 0 );
}

void copyo( char *line )
{
  copy_a_file( line, next_item(line), 1 );
}

void allow_msgs( int up )
{
  int h1, h2;

  if(up) wind_update( END_UPDATE );
/*  for( h1=10; --h1 >= 0; )
    graf_mkstate( &h2, &h2, &h2, &h2 ); */
  evnt_timer( 500, 0 );
  if(up) wind_update( BEG_UPDATE );
}

void destsel( char *line )
{
  char *d=line, dum[13] = "";
  int button, i;

/**  while( *line && !is_sep(*line) ) line++;
  *line++ = '\0';
  while( is_sep(*line) ) line++; **/
  line = next_item(line);
  strcpy( dest_d, d );
  for(;;)
  {
    i = _GemParBlk.global[0] >= 0x0140 ? fsel_exinput( dest_d, dum,
        &button, line ) : fsel_input( dest_d, dum, &button );
    allow_msgs(1);
    objc_draw( status, 0, 8, 0, 0, 32767, 32767 );
    if( !i || button==0 )
    {
      if( var_alert(quitask)==1 ) quitit(0);
    }
    else
    {
      if( (i=pathend(dest_d)) != 0 ) dest_d[i] = '\0';
      return;
    }
  }
}

void bconws( char *ptr )
{
  while( *ptr ) Bconout(2,*ptr++);
}

void run( int tos, char *line )
{
  int h1, h2, h3, h4;
  char *ptr, *ptr2, temp[128];

  flush(line);
  ptr = line;
  while( *ptr && !is_sep(*ptr) ) ptr++;
  if( *ptr )
  {
    *ptr++ = '\0';
    while( is_sep(*ptr) ) ptr++;
  }
  strcpy( temp+1, ptr );
  temp[0] = strlen(ptr);
  if( tos )
  {
    m_off();
    bconws( "\033E\033e\033v" );
  }
  else bee();
  if( not_first ) Mfree(c_buf);
  if( (ptr2=strrchr(line,'\\')) != 0 ) *ptr2++ = '\0';
  else ptr2=line;
  wind_get( 0, WF_WORKXYWH, &h1, &h2, &h3, &h4 );
  blank[0].ob_width = blank[1].ob_width = h3;
  blank[0].ob_height = h2+h4;
  blank[1].ob_height = h2-1;
  check_dir(line);      /* sets the path */
  wind_update( END_UPDATE );
  if( !tos )
  {
    wind_set( 0, WF_NEWDESK, blank );
    form_dial( FMD_FINISH,  0, 0, 0, 0,  0, 0, h1+h3, h2+h4 );
    allow_msgs(0);
  }
  Pexec( 0, ptr2, temp, 0L );
  if( not_first ) copy_init();
  if( tos )
  {
    bconws( "\r\n\n" );
    bconws( runmsg );
    Bconin(2);
    bconws( "\033f\033E" );
    m_on();
  }
  else
  {
    wind_set( 0, WF_NEWDESK, 0L );
    arrow();
  }
  form_dial( FMD_FINISH,  0, 0, 0, 0,  0, 0, h1+h3, h2+h4 );
  allow_msgs(0);
/*  objc_draw( blank, 0, 8, 0, 0, h1+h3, h2+h4 );*/
  dial = -1;
  arrow();
  wind_update( BEG_UPDATE );
}

void tos( char *line )
{
  run( 1, line );
}

void gem( char *line )
{
  run( 0, line );
}

void rm( char *line )
{
  Fdelete(line);
}

void rmdir( char *line )
{
  Ddelete(line);
}

void doopen( char *line )
{
  txtptr = txtbuf;
  txtlen = 0;
  line++;
}

void dowrite( char *line )
{
  int i;

  insert_vars( txtptr, line );
  txtptr += (i=strlen(txtptr));
  if( (txtlen+=i) > TXTBUFLEN-80 ) fatal( "[1][Text file|too large][Ok]", 0 );
  else
  {
    txtlen += 2;
    *txtptr++ = '\r';
    *txtptr++ = '\n';
  }
}

void doclose( char *line )
{
  char *ptr, temp[120];
  int i, l;

  if( !txtlen ) return;
  if( !Fsfirst(line,0x37) )
  {
    strcpy( temp, line );
    i = pathend(temp);
    if( (ptr=strchr(temp+i,'.')) != 0 ) i = ptr-temp;
    else i = strlen(temp);
    strcpy( temp+i, ".OLD" );
    Fdelete(temp);
    Frename( 0, line, temp );
  }
  if( TOS_error( i=Fcreate(line,0) ) )
    if( TOS_error( l = Fwrite( i, txtlen, txtbuf ) ) )
      if( l != txtlen ) var_alert( diskfull );
  txtlen = 0;
}

void addvar( char *line )
{
  if( nvar==5-1 ) fatal( "[1][Too many VARs][OK]", 0 );
  strcpy( tvar[nvar++], line );
}

typedef struct
{
  char lbl[8];
  void (*func)( char *line );
} CMDPTR;
CMDPTR cmdptr[]={{"ASKDRIVE", askdrive },
                { "ASK", ask },
                { "ASKEND", askend },
                { "BUTTON1", button1 },
                { "BUTTON2", button2 },
                { "BUTTON3", button3 },
                { "ECHO", echo },
                { "COPY", copy },
                { "COPYO", copyo },
                { "PAUSE", pause },
                { "MORE", more },
                { "EXIT", doexit },
                { "FLUSH", flush },
                { "IFEX", ifex },
                { "IFNEX", ifnex },
                { "IFEND", ifend },
                { "SWAPSRC", swapsrc },
                { "SWAPDEST", swapdest },
                { "BELL", bell },
                { "MKDIR", mkdir },
                { "TOS", tos },
                { "GEM", gem },
                { "RM", rm },
                { "RMDIR", rmdir },
                { "UNTILEX", untilex },
                { "OPEN", doopen },
                { "WRITE", dowrite },
                { "CLOSE", doclose },
                { "SETDEST", setdest },
                { "VAR", addvar },
                { "DESTSEL", destsel }, 0L };

void fatal( char *s, int val )
{
  char buf[140];

  sprintf( buf, s, val );
  form_alert( 1, buf );
  quitit(-1);
}

void main()
{
  char *line;
  int dx, dy, dw, dh, sx, sy, sw, sh, ask_lev=0, if_lev=0;
  STRPTR *sptr;
  CMDPTR *cmd;

  io = Iorec(1);
  appl_init();
  wind_update( BEG_UPDATE );
  graf_mouse( ARROW, 0L );
  rsrc_init();
  Supexec( (long (*)())get_nflops );
  instdir[0] = Dgetdrv()+'A';
  instdir[1] = ':';
  Dgetpath( instdir+2, 0 );
  strcat( instdir, "\\" );Bconws(instdir);
  dx = 'A'+nflops-1;
  if( (src_d[0] = Dgetdrv()+'A') > dx ) src_d[0] = 'A';
  if( dest_d[0] > dx ) dest_d[0] = dx;
  form_center( status=rs_trindex[STATUS], &sx, &sy, &sw, &sh );
  form_center( disks=rs_trindex[DISKS], &dx, &dy, &dw, &dh );
  set_buttons( "" );
  while( (line=get_line()) != 0 )
    if( *line )
    {
      while( is_sep( *line ) ) line++;
      for( sptr=&strptr[0]; sptr->lbl[0]; sptr++ )
        if( !strxcmp( sptr->lbl, line, 8 ) )
        {
          if( !skipping )
          {
            while( *line && !is_sep(*line) ) line++;
            while( is_sep(*line) ) line++;
            if( sptr->tree<0 ) strncpy( sptr->str, line, sptr->len );
            else
            {
              set_string( sptr->tree, (int)(sptr->str), line );
              if( strlen(line) > sptr->len ) *(line+sptr->len) = '\0';
            }
          }
          goto found;
        }
      for( cmd=&cmdptr[0]; cmd->lbl[0]; cmd++ )
        if( !strxcmp( cmd->lbl, line, 8 ) )
        {
          if( skipping )
            if( cmd->func==ask ) ask_lev++;
            else if( cmd->func==askend && ask_lev )
            {
              ask_lev--;
              goto found;
            }
            else if( cmd->func==ifex || cmd->func==ifnex ) if_lev++;
            else if( cmd->func==ifend && if_lev )
            {
              if_lev--;
              goto found;
            }
          if( !ask_lev && !if_lev && (!skipping || cmd->func==askend ||
              cmd->func==button1 || cmd->func==button2 ||
              cmd->func==button3 || cmd->func==ifend) )
          {
            while( *line && !is_sep(*line) ) line++;
            if( cmd->func == echo ) line++;
            else while( is_sep(*line) ) line++;
            if( dial!=STATUS )
            {
              dial = STATUS;
              objc_draw( status, 0, 8, sx, sy, sw, sh );
            }
            insert_vars( bigbuf, line );
            (*cmd->func)( bigbuf );
            if( cmd->func == askdrive )
              objc_draw( status, 0, 8, dx, dy, dw+2, dh+2 );
          }
          goto found;
        }
      fatal( "[1][Unknown command in|script file at line %d][Ok]", line_num );
found:
        ;
    }
  quitit(0);
}
