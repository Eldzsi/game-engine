local elementData = {}

function setElementProperty(theElement, customDataName, value)
    if not elementData[theElement] then
        elementData[theElement] = {}
    end
    elementData[theElement][customDataName] = value
end

function getElementProperty(theElement, customDataName)
    if elementData[theElement] then
        return elementData[theElement][customDataName]
    end
    return nil
end

function clearElementData(theElement)
    if elementData[theElement] then
        elementData[theElement] = nil
        return true
    end
    return false
end
