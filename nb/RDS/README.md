### RDS decoding

In order:
  * _AM_ file:
    * we compare different AM demodulation methods to obtain RDS modulating (data) signal.
  * _symbol_sync_ file:
    * we do our best to decode actual binary symbols from it. Symbol synchronization is the kicker.
  * _data_link_ file:
    * we apply error correction and form data groups.
    
And finally, we shall map these group onto actual human readable symbols.
