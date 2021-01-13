#include <stdio.h>
#include <stdlib.h>

#define CMDLINE_SIZE 4096
#define TAB 0x09
#define LF 0x0a
#define CR 0x0d

/*
 * Given a specific label from the kernel command line (cat /proc/cmdline) as input,
 * this program will read the kernel command line searching for the specified
 * label and output the associated data. If the data associated with the label is
 * double quoted then the data can include embedded whitespace characters.
 * If not quoted then only the first word after the = symbol is read.
 * 
 * syntax:
 * kernel_cmdline_extractor label
 * 
 * Example:
 * If the kernel command line = linux /boot/shredos console=tty3 loglevel=3 shredos=autonuke loadkeys=uk
 * 
 * then
 * 
 * kernel_cmdline_extractor shredos would output 'autonuke' to stdout.
 * 
 * while
 * 
 * kernel_cmdline_extractor loadkeys would output 'uk' to stdout.
 *
 * Return codes:
 * 0 = success
 * 1 = unable to open /proc/cmdline
 * 2 = unable to read /proc/cmdline
 * 3 = No label specified by user
 * 4 = label not found in kernel command line
 * 5 = A CMDLINE_SIZE, typically 4096 bytes, kernel command line was actually read. Not very likely!
 * 
 */

int main(int argc, char **argv) {
   
   /* Our raw kernel_cmdline is stored here */
   char kernel_cmdline[CMDLINE_SIZE];
   
   /* start of the label that matches the label requested */
   char *start_of_label = 0;
   
   int elements_requested = 1;
   
   /* generic index counters */
   int idx;
   int idx2;
   
   /* A flag that indicates the requested label has been found */
   int label_found = 0;
   
   /* return status of fread, I don't actually need this but it gets rid of compiler warning */
   size_t status;
   
   /* pointer to start of data that will be output on stdout */
   char * output_data;
   
   /* The log file pointer. */
   FILE* fp;
   
   /* Initialiase the kernel command line buffer */
   kernel_cmdline[0] = 0;
   
   if ( argc != 2 )
   {
      printf ( "No label specified\n" );
      return 3;
   }
   
   /* Open a stream to /proc/cmdline */
   //fp = fopen( "/proc/cmdline", "r" );
   fp = fopen( "/home/nick/Data/tmp/grub.cfg", "r" );
   
   if ( fp == NULL )
   {
      return 1;
   }
   
   /* Read the command line from /proc/cmdline */
   status = fread( kernel_cmdline, CMDLINE_SIZE, elements_requested, fp );
   
   if ( status == elements_requested )
   {
      printf(" The kernel command line is exactly %d, very unlikely!", CMDLINE_SIZE );
   }
   
   /* terminate the very end of the buffer to avoid any buffer overrun */
   kernel_cmdline[ CMDLINE_SIZE -1 ] = 0;
   
   /* Did we read something? */
   idx = 0;
   idx2 = 0;
   if( kernel_cmdline[0] != 0 )
   {
      /* start searching for the label */
      while ( idx < CMDLINE_SIZE )
      {
         if ( kernel_cmdline[ idx++ ] == *(argv[1]+idx2) )
         {
            if ( ! start_of_label )
            {
               /* Notes the start address of the label */
               start_of_label = &kernel_cmdline[ idx-1 ];
            }
            idx2++;
            /* Check for end of the label */
            if ( *(argv[1]+idx2) == 0 )
            {
               /* Skip any spaces before = symbol */
               while ( ( kernel_cmdline[ idx ] == ' ' || kernel_cmdline[ idx ] == TAB ) && kernel_cmdline[ idx ] != 0 )
               {
                  idx++;
               }
               if (kernel_cmdline[ idx ] == '=' )
               {
                  idx++;
                  label_found = 1;
                  break;
               }
               else
               {
                  /* reset the address that points to the start of the label */
                  start_of_label = 0;
               }
            }
         }
         else
         {
            idx2=0;
            start_of_label = 0;
         }
      }
      if ( label_found == 0 )
      {
         printf ( "Label not found in kernel command line\n" );
         fclose( fp );
         return 4;
      }
      
      /* if there are any spaces following the found label's = symbol then skip them */
      while ( ( kernel_cmdline[ idx ] == ' ' || kernel_cmdline[ idx ] == TAB ) && kernel_cmdline[ idx ] != 0 )
      {
         idx++;
      }

      /* Is the first character after the 'label=' a double quote? i.e "--method=dod --rounds=2" 
       * OR does the label start with a double quote? This code may look complicated but it has to
       * deal with the case where the user edits the kernel command line with say nwipe_options="--nousb --method=zero"
       * bit by the time it appears in /proc/cmdline it has changed to "nwipe_options=--nousb --method=zero" where the
       * leading " has been moved either by grub or the kernel to the start of nwipe_options. This code deals with that
       * situation.
      */
      if ( ( kernel_cmdline[ idx ] == '"' || *(start_of_label-1) == '"' ) && kernel_cmdline[ idx ] != 0 )
      {
         if ( kernel_cmdline[ idx ] == '"' )
         {
            idx++;
         }
         
         /* record start address of data */
         output_data = kernel_cmdline+idx;
         
         /* scan for the closing double quote " */
         while  ( kernel_cmdline[ idx ] != '"' && kernel_cmdline[ idx ] != 0 )
         {
            idx++;
         }
      }
      else /* Just read the first word */
      {
         /* record start address of data */
         output_data = kernel_cmdline+idx;
         
         while ( kernel_cmdline[ idx ] != ' ' && kernel_cmdline[ idx ] != TAB && kernel_cmdline[ idx ] != LF && kernel_cmdline[ idx ] != CR )
         {
            idx++;
         }
      }
      /* terminate the data section, no need to terminate if the
       * index has reached the end of the buffer as the end of the 
       * buffer was previosly terminated */
      if ( idx < CMDLINE_SIZE )
      {
         kernel_cmdline[ idx ] = 0;
      }

      /* output the data to stdout */
      printf ("%s\n", output_data );
      
      fclose( fp );

      return 0;
   }

   fclose( fp );

   return 2;
}

