void enable_paging() {
    // TODO: Homework 2: Enable paging
    // After initializing the page table, write to register SATP register for kernel registers.
    // Flush the TLB to invalidate the existing TLB Entries
    w_satp(MAKE_SATP(kernel_pagetable));
    flush_tlb();
    // Suggested: 2 LoCs
}

// Return the address of the PTE in page table *pagetable*
// The Risc-v Sv48 scheme has four levels of page table.
// For VA:
//   47...63 zero
//   39...47 -- 9  bits of level-3 index
//   30...38 -- 9  bits of level-2 index
//   21...29 -- 9  bits of level-1 index
//   12...20 -- 9  bits of level-0 index
//   0...11  -- 12 bits of byte offset within the page
// Return the last-level page table entry.
static pte_t* pt_query(pagetable_t pagetable, vaddr_t va, int alloc){
    if(va >= MAXVA) BUG_FMT("get va[0x%lx] >= MAXVA[0x%lx]", va, MAXVA);
    for (int i=3;i;i--){
    	pte_t* tmp=&pagetable[PX(i,va)];
    	if ((*tmp)&PTE_V) pagetable=(pagetable_t)PTE2PA(*tmp); else
    	{
    		if ((alloc==0)||((pagetable=(mm_kalloc()))==0)) return 0;
    		memset(pagetable,0,PGSIZE);
    		*tmp=(PA2PTE(pagetable))|PTE_V;
    	}
    }
    return &pagetable[PX(0,va)];
    // Suggested: 18 LoCs
}
int pt_map_pages(pagetable_t pagetable, vaddr_t va, paddr_t pa, uint64 size, int perm){
	uint64 tmp=PGROUNDDOWN(va),last=PGROUNDDOWN(va+size-1);
	pte_t* pte;
	for(;tmp<=last;tmp+=PGSIZE,pa+=PGSIZE){
		pte=pt_query(pagetable,tmp,1);
		if (pte==0) return -1;
		*pte=PA2PTE(pa)|perm|PTE_V;
	}
    // Suggested: 11 LoCs
    return 0; // Do not modify
}

paddr_t pt_query_address(pagetable_t pagetable, vaddr_t va){
	if (va>=MAXVA) return 0;
	pte_t* pte=pt_query(pagetable,va,0);
	if (pte==0) return 0;
    // Suggested: 3 LoCs
    return PTE2PA(*pte)|(va&0xfff);
}

int pt_unmap_addrs(pagetable_t pagetable, vaddr_t va){
	pte_t* pte=pt_query(pagetable,PGROUNDDOWN(va),0);
	*pte=0;
    // Suggested: 2 LoCs
    return 0; // Do not modify
}

int pt_map_addrs(pagetable_t pagetable, vaddr_t va, paddr_t pa, int perm){
	pt_map_pages(pagetable,va,pa,PGSIZE,perm);
    // Suggested: 2 LoCs
    return 0; // Do not modify
}
