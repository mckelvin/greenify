# coding: utf-8

# greenify
import greenify
greenify.greenify()

# python patch
import gevent
import gevent.monkey
gevent.monkey.patch_all()

import sys
import time
import mod_http_head
greenify.patch_lib(mod_http_head.__file__)
reload(mod_http_head)


def c_http_head_check(host):
    print >> sys.stderr, '%.5f head %s begin' % (time.time(), host)
    ret = mod_http_head.http_head(host)
    print >> sys.stderr, '%.5f head %s end' % (time.time(), host)
    assert ret == 1


def python_http_head_check(host):
    import httplib
    print >> sys.stderr, '%.5f head %s begin' % (time.time(), host)
    conn = httplib.HTTPConnection(host)
    conn.request("HEAD", "/")
    resp = conn.getresponse()
    status_code = resp.status
    print >> sys.stderr, '%.5f head %s end' % (time.time(), host)
    assert 200 <= status_code < 400


def main():
    for fn in [python_http_head_check, c_http_head_check]:
        print 'test %s' % fn.__name__
        t0 = time.time()
        workers = [
            gevent.spawn(fn, host)
            for host in ("msn.com", "baidu.com", "douban.fm", "tumblr.com")
        ]
        gevent.joinall(workers)
        print 'done in %.5fs' % (time.time() - t0)
        print


if __name__ == '__main__':
    main()
