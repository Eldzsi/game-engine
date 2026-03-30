local _destroyElement = destroyElement

function destroyElement(id)
    if not id then return false end

    if clearElementData then 
        clearElementData(id) 
    end

    return _destroyElement(id)
end
