.\debug\sw28027.out
-m
-memwidth 16
-romwidth 16
-o .\debug\sw28027.mhx
SECTIONS
{
        
         codestart, .text,  .cinit, .pinit, .econst, .switch, .reset, ramfuncs, IQmath
}