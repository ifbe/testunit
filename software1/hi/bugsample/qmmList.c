/*==========================================================================
     
         FUNCTION:      qmm_ListSize
                                                                        
         DESCRIPTION:                                                       
*//**       This function will return the total number of nods in the
            given list.

@par     DEPENDENCIES:   
               None
*//*                                                                        
         PARAMETERS:
*//**       @param[in]  pList       - Pointer to the list object.
            @param[in]  fnCmp       - Function which implements comparison
                                      logic for the given comparison
                                      value.
            @param[in] pCmpValue    - Pointer to the value which is used for 
                                      comparison against the given element's
                                      value.
            @param[out] ppSearchLink -
                                      Pointer to a pointer to the variable of
                                      link type which will be filled with
                                      pointer to the searched element.
                     
**//*     RETURN VALUE:                                                                   
*//**       @return    QMM Error code 

@par     SIDE EFFECTS:   
                        None
                                                                        
===========================================================================*/
