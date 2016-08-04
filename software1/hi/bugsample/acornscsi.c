const
char *acornscsi_info(struct Scsi_Host *host)
{
    static char string[100], *p;

    p = string;

    p += sprintf(string, "%s at port %08lX irq %d v%d.%d.%d"
#ifdef CONFIG_SCSI_ACORNSCSI_SYNC
    " SYNC"
#endif
#ifdef CONFIG_SCSI_ACORNSCSI_TAGGED_QUEUE
    " TAG"
#endif
#if (DEBUG & DEBUG_NO_WRITE)
    " NOWRITE (" __stringify(NO_WRITE) ")"
#endif
                , host->hostt->name, host->io_port, host->irq,
                VER_MAJOR, VER_MINOR, VER_PATCH);
    return string;
}

