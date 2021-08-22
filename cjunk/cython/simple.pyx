cdef unique_function():
    return 100

cdef second_function():
    cdef a=0
    for i in range(100):
        a+=i

cdef class a_class:
    cdef memberA(self):
        return self.__name__
print(second_function())
