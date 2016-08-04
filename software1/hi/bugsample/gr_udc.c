static void gr_free_dma_desc_chain(struct gr_udc *dev, struct gr_request *req)
{
        struct gr_dma_desc *desc;
        struct gr_dma_desc *next;

        next = req->first_desc;
        if (!next)
                return;

        do {
                desc = next;
                next = desc->next_desc;
                gr_free_dma_desc(dev, desc);
        } while (desc != req->last_desc);

        req->first_desc = NULL;
        req->curr_desc = NULL;
        req->last_desc = NULL;
}
static void gr_ep0_setup(struct gr_udc *dev, struct gr_request *req);

/*
 * Frees allocated resources and calls the appropriate completion function/setup
 * package handler for a finished request.
 *
 * Must be called with dev->lock held and irqs disabled.
 */
static void gr_finish_request(struct gr_ep *ep, struct gr_request *req,
                              int status)
        __releases(&dev->lock)
        __acquires(&dev->lock)
{
	list_del_init(&req->queue);
}
