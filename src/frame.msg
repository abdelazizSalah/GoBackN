
packet Frame {
    @customize(true);
	unsigned int Header;
	string Payload;
	char Trailer;
	int FrameType;
    unsigned int AckNum;
}