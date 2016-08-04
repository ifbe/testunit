static void h5_complete_rx_pkt(struct hci_uart *hu)
{
        struct h5_struct *h5 = hu->priv;
        int pass_up;

        if (h5->rx_skb->data[0] & 0x80) {       /* reliable pkt */
                BT_DBG("Received seqno %u from card", h5->rxseq_txack);
                h5->rxseq_txack++;
                h5->rxseq_txack %= 0x8;
                h5->txack_req    = 1;

                /* If needed, transmit an ack pkt */
                hci_uart_tx_wakeup(hu);
        }

        h5->rxack = (h5->rx_skb->data[0] >> 3) & 0x07;
        BT_DBG("Request for pkt %u from card", h5->rxack);

        h5_pkt_cull(h5);

        if ((h5->rx_skb->data[1] & 0x0f) == 2 &&
                        h5->rx_skb->data[0] & 0x80) {
                bt_cb(h5->rx_skb)->pkt_type = HCI_ACLDATA_PKT;
                pass_up = 1;
        } else if ((h5->rx_skb->data[1] & 0x0f) == 4 &&
                        h5->rx_skb->data[0] & 0x80) {
                bt_cb(h5->rx_skb)->pkt_type = HCI_EVENT_PKT;
                pass_up = 1;
        } else if ((h5->rx_skb->data[1] & 0x0f) == 3) {
                bt_cb(h5->rx_skb)->pkt_type = HCI_SCODATA_PKT;
                pass_up = 1;
        } else if ((h5->rx_skb->data[1] & 0x0f) == 15 &&
                        !(h5->rx_skb->data[0] & 0x80)) {
                //h5_handle_le_pkt(hu);//Link Establishment Pkt
                pass_up = 0;
        } else if ((h5->rx_skb->data[1] & 0x0f) == 1 &&
                        h5->rx_skb->data[0] & 0x80) {
                bt_cb(h5->rx_skb)->pkt_type = HCI_COMMAND_PKT;
                pass_up = 1;
        } else if ((h5->rx_skb->data[1] & 0x0f) == 14) {
                bt_cb(h5->rx_skb)->pkt_type = H5_VDRSPEC_PKT;
                pass_up = 1;
        } else
                pass_up = 0;

        if (!pass_up) {
#if 0
                struct hci_event_hdr hdr;
#endif
                u8 desc = (h5->rx_skb->data[1] & 0x0f);

                if (desc != H5_ACK_PKT && desc != H5_LE_PKT) {
#if 0
                        if (hciextn) {
                                desc |= 0xc0;
                                skb_pull(h5->rx_skb, 4);
                                memcpy(skb_push(h5->rx_skb, 1), &desc, 1);

                                hdr.evt = 0xff;
                                hdr.plen = h5->rx_skb->len;
                                memcpy(skb_push(h5->rx_skb, HCI_EVENT_HDR_SIZE), &hdr, HCI_EVENT_HDR_SIZE);
                                bt_cb(h5->rx_skb)->pkt_type = HCI_EVENT_PKT;

                                hci_recv_frame(h5->rx_skb);
                        } else {
#endif
                                BT_ERR ("Packet for unknown channel (%u %s)",
                                        h5->rx_skb->data[1] & 0x0f,
                                        h5->rx_skb->data[0] & 0x80 ?
                                        "reliable" : "unreliable");
                                kfree_skb(h5->rx_skb);
//                      }
                } else
                        kfree_skb(h5->rx_skb);
        } else {
                /* Pull out H5 hdr */
                skb_pull(h5->rx_skb, 4);

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0)
                hci_recv_frame(h5->rx_skb);
#else
                hci_recv_frame(hu->hdev, h5->rx_skb);
#endif
        }

        h5->rx_state = H5_W4_PKT_DELIMITER;
        h5->rx_skb = NULL;
}

