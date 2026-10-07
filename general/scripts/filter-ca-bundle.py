#!/usr/bin/env python3
"""
Cut a PEM CA bundle down to the roots of named CA operators, in place.

    filter-ca-bundle.py KEEP_LIST BUNDLE

KEEP_LIST holds one subject O= prefix per line; '#' comments and blank lines
are skipped. A certificate is kept when any O= in its subject starts with any
entry. Kept certificates are written back byte for byte and in their original
order: majestic refuses to start if a single certificate in the bundle fails
to parse, so this must only ever take certificates away.

Exits non-zero, leaving BUNDLE untouched, when an entry matches no certificate
or nothing is kept. Run by rootfs_script.sh for lite images (#2508).

Standard library only: the subject is read by walking the DER directly, so
the build needs neither the openssl CLI nor a Python package.
"""

import base64
import re
import sys

OID_ORGANIZATION = bytes.fromhex("060355040a")  # 2.5.4.10
# DirectoryString is a CHOICE. Mozilla's roots are all UTF8String or
# PrintableString today, but a name in another encoding must still match: a
# kept operator's new root that failed to would be dropped without a word.
STRING_CODECS = {
    0x0C: "utf-8",          # UTF8String
    0x13: "ascii",          # PrintableString
    0x14: "latin-1",        # TeletexString, as everyone reads it in practice
    0x16: "ascii",          # IA5String
    0x1C: "utf-32-be",      # UniversalString
    0x1E: "utf-16-be",      # BMPString
}
PEM = re.compile(rb"-----BEGIN CERTIFICATE-----\r?\n.*?-----END CERTIFICATE-----\r?\n?", re.S)


def tlv(der, pos):
    """Return (tag, value start, value end) of the DER element at pos."""
    tag, length = der[pos], der[pos + 1]
    pos += 2
    if length & 0x80:
        count = length & 0x7F
        length = int.from_bytes(der[pos:pos + count], "big")
        pos += count
    return tag, pos, pos + length


def subject_organizations(der):
    _, pos, _ = tlv(der, 0)                 # Certificate
    _, pos, _ = tlv(der, pos)               # TBSCertificate
    if der[pos] == 0xA0:                    # [0] version, absent on v1
        pos = tlv(der, pos)[2]
    for _ in range(4):                      # serial, signature, issuer, validity
        pos = tlv(der, pos)[2]
    _, pos, end = tlv(der, pos)             # subject
    orgs = []
    while pos < end:
        _, attr, pos = tlv(der, pos)        # RelativeDistinguishedName SET
        # A SET may carry several attributes, the O= not necessarily first.
        while attr < pos:
            _, inner, attr = tlv(der, attr) # AttributeTypeAndValue
            _, _, value = tlv(der, inner)
            if der[inner:value] == OID_ORGANIZATION:
                tag, start, stop = tlv(der, value)
                orgs.append(der[start:stop].decode(STRING_CODECS.get(tag, "utf-8"), "replace"))
    return orgs


def main(keep_path, bundle_path):
    with open(keep_path, encoding="utf-8") as f:
        keep = [line.strip() for line in f]
    keep = [k for k in keep if k and not k.startswith("#")]

    with open(bundle_path, "rb") as f:
        certs = PEM.findall(f.read())

    hits = dict.fromkeys(keep, 0)
    kept = []
    for pem in certs:
        body = b"".join(pem.splitlines()[1:-1])
        orgs = subject_organizations(base64.b64decode(body))
        matched = [k for k in keep if any(o.startswith(k) for o in orgs)]
        for k in matched:
            hits[k] += 1
        if matched:
            kept.append(pem if pem.endswith(b"\n") else pem + b"\n")

    stale = [k for k, n in hits.items() if n == 0]
    for k in stale:
        print(f"ca-bundle: '{k}' matches no root in {bundle_path}", file=sys.stderr)
    if stale or not kept:
        print(f"ca-bundle: refusing to write {bundle_path}; fix {keep_path}", file=sys.stderr)
        return 1

    with open(bundle_path, "wb") as f:
        f.write(b"".join(kept))
    print(f"ca-bundle: kept {len(kept)} of {len(certs)} roots")
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit("usage: filter-ca-bundle.py KEEP_LIST BUNDLE")
    sys.exit(main(sys.argv[1], sys.argv[2]))
