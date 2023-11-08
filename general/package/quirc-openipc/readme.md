
### Sample usage

```
#!/bin/sh

while true ; do
    sleep 1
    curl -s -o /tmp/img.jpg http://127.0.0.1/image.jpg ; qrscan -p /tmp/img.jpg
done
```
