def nome_hash(cont_hash):
    import hashlib
    _hash = hashlib.md5()
    _hash.update(cont_hash.encode('utf-8'))
    return (_hash.hexdigest())


# %%
if __name__ == "__main__":
    import os
    path = os.getcwd()
    hexa = nome_hash(path)
    print(hexa)