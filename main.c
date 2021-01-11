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
 * kernel_cmdline_extractor(shredos) would output 'autonuke' to stdout.
 * 
 * while
 * 
 * kernel_cmdline_extractor(loadkeys) would output 'uk' to stdout.
 *
 * Return codes:
 * 0 = success
 * 1 = unable to open /proc/cmdline
 * 2 = unable to read /proc/cmdline
 * 3 = No label specified by user
 * 4 = label not found in kernel command line
 * 
 */

int main(int argc, char **argv) {
   
   char kernel_cmdline[CMDLINE_SIZE];
   
   int elements_requested = 1;
   int idx;
   int idx2;
   int label_found = 0;
   
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
   fp = fopen( "/proc/cmdline", "r" );
   
   if ( fp == NULL )
   {
      return 1;
   }
   
   /* Read the command line from /proc/cmdline */
   fread( kernel_cmdline, CMDLINE_SIZE, elements_requested, fp );
   
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
            idx2++;
            /* Check for end of the label */
            if ( *(argv[1]+idx2) == 0 )
            {
               /* Skip any spaces before = symbol */
               while ( kernel_cmdline[ idx ] == ' ' || kernel_cmdline[ idx ] == 0 || kernel_cmdline[ idx ] == TAB )
               {
                  idx++;
               }
               if (kernel_cmdline[ idx ] == '=' )
               {
                  idx++;
                  label_found = 1;
                  break;
               }
            }
         }
         else
         {
            idx2=0;
         }
      }
      if ( label_found == 0 )
      {
         printf ( "Label not found in kernel command line\n" );
         fclose( fp );
         return 4;
      }
      
      /* if there are any spaces following the found label's = symbol then skip them */
      while ( kernel_cmdline[ idx ] == ' ' && kernel_cmdline[ idx ] != 0 )
      {
         idx++;
      }
      
      /* record start address of data */
      output_data = kernel_cmdline+idx+1;

      /* Is the first character a double quote? i.e "--method=dod --rounds=2" */
      if ( kernel_cmdline[ idx ] == '"' && kernel_cmdline[ idx ] != 0 )
      {
         idx++;
         
         /* scan for the closing double quote " */
         while  ( kernel_cmdline[ idx ] != '"' && kernel_cmdline[ idx ] != 0 )
         {
            idx++;
         }
      }
      else /* Just read the first word */
      {
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

/* INFO for call scrit for use in script https://stackoverflow.com/questions/36921658/save-command-output-on-variable-and-check-exit-status )
 * so i can assign the output of this program to a bash variable and check for return value of 0 
 */
