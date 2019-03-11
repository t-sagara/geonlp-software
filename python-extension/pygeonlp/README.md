## Install

```sh
$ python setup.py install
```

### Test Run

```sh
$ python
>>> import practice as p
>>> p.hello_world()
Hello World!
>>> exit()
```

## Uninstall

```sh
$ python setup.py install --record files.txt
$ cat files.txt | xargs rm -rvf
```
