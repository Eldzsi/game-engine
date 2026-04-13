local isConsoleActive = false
local consoleText = ""

local logLines = {}
local maxLogLines = 15
local commandHistory = {}
local historyPos = 0

local function addLog(text)
    table.insert(logLines, text)
    if #logLines > maxLogLines then
        table.remove(logLines, 1)
    end
end

addLog("Console")

addEventHandler("onTextInput", function(text)
    if isConsoleActive then
        consoleText = consoleText .. text
    end
end)

addEventHandler("onKey", function(keyName, isPressed)
    if not isPressed then return end

    if keyName == "F12" then
        isConsoleActive = not isConsoleActive
        showCursor(isConsoleActive)
        return
    end

    if isConsoleActive then
        if keyName == "Backspace" then
            if string.len(consoleText) > 0 then
                consoleText = string.sub(consoleText, 1, -2)
            end
        
        elseif keyName == "Up" then
            if #commandHistory > 0 and historyPos > 1 then
                historyPos = historyPos - 1
                consoleText = commandHistory[historyPos]
            end
            
        elseif keyName == "Down" then
            if historyPos < #commandHistory then
                historyPos = historyPos + 1
                consoleText = commandHistory[historyPos]
            else
                historyPos = #commandHistory + 1
                consoleText = ""
            end

        elseif keyName == "Enter" or keyName == "Return" then
            if string.len(consoleText) > 0 then
                local cmd = consoleText
                addLog("] " .. cmd)
                
                if commandHistory[#commandHistory] ~= cmd then
                    table.insert(commandHistory, cmd)
                end
                historyPos = #commandHistory + 1
                
                local func, err = load("return " .. cmd, "console", "t", _G)
                
                if not func then
                    func, err = load(cmd, "console", "t", _G)
                end
                
                if func then
                    local results = table.pack(pcall(func))
                    local success = results[1]
                    
                    if success then
                        if results.n > 1 then
                            local outStrs = {}
                            for i = 2, results.n do
                                table.insert(outStrs, tostring(results[i]))
                            end
                            addLog("= " .. table.concat(outStrs, ", "))
                        end
                    else
                        addLog("RUNTIME ERROR: " .. tostring(results[2]))
                    end
                else
                    addLog("SYNTAX ERROR: " .. tostring(err))
                end
                
                consoleText = ""
            end
        end
    end
end)

addEventHandler("onPostRender", function()
    if isConsoleActive then
        local w, h = getScreenSize()
        local lineHeight = 22
        local padding = 10
        local totalHeight = (maxLogLines + 1) * lineHeight + (padding * 2)
        
        drawRectangle(0, 0, w, totalHeight, 0, 0, 0, 0.85)
        
        local currentY = padding
        for i, line in ipairs(logLines) do
            local r, g, b = 1, 1, 1
            if string.sub(line, 1, 5) == "ERROR" or string.find(line, "ERROR:") then
                r, g, b = 1, 0.3, 0.3
            elseif string.sub(line, 1, 2) == "= " then
                r, g, b = 0.5, 1, 0.5
            elseif string.sub(line, 1, 2) == "] " then
                r, g, b = 0.8, 0.8, 0.8
            end

            drawText(line, padding, currentY, "assets/fonts/montserrat.ttf", 18, r, g, b, 1, "left")
            currentY = currentY + lineHeight
        end
        
        local displayText = "> " .. consoleText .. "_"
        drawText(displayText, padding, currentY, "assets/fonts/montserrat.ttf", 18, 1, 1, 0, 1, "left")
    end
end)
