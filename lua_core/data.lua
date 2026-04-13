local elementData = {}

function setElementData(theElement, customDataName, value)
    if not elementData[theElement] then
        elementData[theElement] = {}
    end
    elementData[theElement][customDataName] = value
end

function getElementData(theElement, customDataName)
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
