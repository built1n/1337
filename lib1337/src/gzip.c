#include "1337_imp.h"

#define GZIP_CHUNK 0x1000

void l_gz_write(const struct interface_t *interface, void *f, const void *buf, size_t sz)
{
    static uchar outbuf[GZIP_CHUNK];

    z_stream out;
    out.zalloc = Z_NULL;
    out.zfree = Z_NULL;
    out.opaque = Z_NULL;
    if(deflateInit(&out, Z_DEFAULT_COMPRESSION) != Z_OK)
    {
        interface->fatal("failed to open gzip stream for output");
    }

    out.avail_in = sz;
    out.next_in = (uchar*)buf;
    do {
        out.avail_out = GZIP_CHUNK;
        out.next_out = outbuf;
        assert(deflate(&out, Z_FINISH) != Z_STREAM_ERROR);
        size_t have = GZIP_CHUNK - out.avail_out;
        if(interface->fwrite(outbuf, have, f) != have || interface->ferror(f))
            interface->fatal("File I/O error");
    } while (out.avail_in > 0);

    deflateEnd(&out);
}

size_t l_gz_read(const struct interface_t *interface, void *f, void *buf, size_t max)
{
    static uchar inbuf[GZIP_CHUNK];

    z_stream in;
    in.zalloc = Z_NULL;
    in.zfree = Z_NULL;
    in.opaque = Z_NULL;
    in.avail_in = 0;
    in.next_in = Z_NULL;

    if(inflateInit(&in) != Z_OK)
        interface->fatal("failed to open gzip stream");

    int ret;
    uchar *out = buf;
    size_t avail = max, total = 0;

    do {
        interface->logf(LOG_DEBUG, "l_gz_read processing chunk of compressed data...\n");
        in.avail_in = interface->fread(inbuf, GZIP_CHUNK, f);
        interface->logf(LOG_DEBUG, "have %d bytes\n", in.avail_in);
        if(!in.avail_in)
            break;
        in.next_in = inbuf;

        do {
            in.avail_out = MIN(GZIP_CHUNK, avail);
            in.next_out = out;
            ret = inflate(&in, Z_NO_FLUSH);

            assert(ret != Z_STREAM_ERROR);

            switch(ret)
            {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR; /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&in);
                interface->fatal("gzip memory error");
            }
            size_t have = GZIP_CHUNK - in.avail_out;
            out += have;
            total += have;
            avail -= have;
        } while (in.avail_out == 0);
    } while (ret != Z_STREAM_END);

    inflateEnd(&in);

    return total;
}
