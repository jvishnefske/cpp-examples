void private_queueInitialize(struct Queue *self, void *pDataArray, uint16_t elementSize, uint16_t elementCapacity)
{
    self->begin = 0u;
    self->end = 0u;
    self->pDataArray = (uint8_t *) pDataArray; // NOLINT (misra-c2012-11.5)
    self->elementSize = elementSize;
    self->elementCapacity = elementCapacity;
    self->isFull = false;
}

static void private_queueEnqueue(struct Queue *self, void *pData)
{
    if (self->isFull)
    {
        return;
    }

    uint16_t index = self->end;
    uint16_t endIndex = (self->begin + self->end) % self->elementCapacity;
    uint16_t availableSpace = self->elementCapacity - endIndex;

    if (availableSpace == 0u)
    {
        self->isFull = true;
        return;
    }

    self->end = (self->end + 1u) % self->elementCapacity;

    memcpy(&self->pDataArray[index * self->elementSize], pData, self->elementSize);
}

static void private_queueDequeue(struct Queue *self, void *pData)
{
    if (self->begin == self->end)
    {
        return;
    }

    uint16_t index = self->begin;
    uint16_t availableSpace = self->end - self->begin;

    if (availableSpace == 0u)
    {
        self->isFull = false;
        return;
    }

    self->begin = (self->begin + 1u) % self->elementCapacity;

    memcpy(pData, &self->pDataArray[index * self->elementSize], self->elementSize);
}

static void private_queueClear(struct Queue *self)
{
    self->begin = 0u;
    self->end = 0u;
    self->isFull = false;
}

static bool private_queueIsEmpty(struct Queue *self)
{
    return (self->begin == self->end);
}

static bool private_queueIsFull(struct Queue *self)
{
    return self->isFull;
}

static uint16_t private_queueGetSize(struct Queue *self)
{
    return (self->end - self->begin);
}

static uint16_t private_queueGetCapacity(struct Queue *self)
{
    return self->elementCapacity;
}

static void *private_queueGetData(struct Queue *self, uint16_t index)
{
    uint16_t begin = self->begin;
    uint16_t end = self->end;
    uint16_t capacity = self->elementCapacity;
    uint16_t elementSize = self->elementSize;

    if (index >= (end - begin))
    {
        return NULL;
    }

    return &self->pDataArray[(begin + index) * elementSize];
}

static void private_queueSetData(struct Queue *self, uint16_t index, void *pData)
{
    uint16_t begin = self->begin;
    uint16_t end = self->end;
    uint16_t capacity = self->elementCapacity;
    uint16_t elementSize = self->elementSize;

    if (index >= (end - begin))
    {
        return;
    }

    memcpy(&self->pDataArray[(begin + index) * elementSize], pData, elementSize);
}

static void private_queueSetCapacity(struct Queue *self, uint16_t elementCapacity)
{
    uint16_t size = self->end - self->begin;

    if (size > elementCapacity)
    {
        return;
    }

    uint16_t newDataArraySize = elementCapacity * self->elementSize;
    uint8_t *pNewDataArray = (uint8_t *) malloc(newDataArraySize);

    if (pNewDataArray == NULL)
    {
        return;
    }

    memcpy(pNewDataArray, self->pDataArray, self->elementSize * self->elementCapacity);

    free(self->pDataArray);

    self->pDataArray = pNewDataArray;
    self->elementCapacity = elementCapacity;
}

static void private_queueSetElementSize(struct Queue *self, uint16_t elementSize)
{
    uint16_t size = self->end - self->begin;

    if (size > self->elementCapacity)
    {
        return;
    }

    uint16_t newDataArraySize = self->elementCapacity * elementSize;
    uint8_t *pNewDataArray = (uint8_t *) malloc(newDataArraySize);

    if (pNewDataArray == NULL)
    {
        return;
    }

    memcpy(pNewDataArray, self->pDataArray, self->elementSize * self->elementCapacity);

    free(self->pDataArray);

    self->pDataArray = pNewDataArray;
    self->elementSize = elementSize;
}

