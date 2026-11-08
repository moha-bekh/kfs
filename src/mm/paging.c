/*
 * KFS-3
 *
 * Role: Enable paging (page directory/tables, CR3, CR0.PG); structure handling paging and memory rights; kernel/user space split; create/get pages.
 * Depends on: pmm
 * Unlocks: kmalloc/vmalloc, process memory (KFS-5)
 */

#include "mm/paging.h"
