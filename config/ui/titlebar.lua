--[[
    This function controls the program's title bar along with the menubar controls underneath.

    This function is not replaceable with a custom function.
]]
function CustomTitleBar()
    local width = Win:GetWidth()

    local titleLabelWidth = width - 100.0
    local titleButtonsWidth = width - 105.0

    -- Shrink the title label if the window is too small.
    if width < 200 then
        titleLabelWidth = width - titleButtonsWidth - 10.0
    end

    if Ctx:Begin("ProtoMapper", new_rect(0.0, 0.0, width, 30.0), PanelFlag.NoScrollbar) then
        
        Ctx:SpaceRowBegin(Layout.Static, 20.0, 4)

        -- The app title.
        Ctx:SpaceRowPush(new_rect(0.0, 0.0, titleLabelWidth, 25.0))
        Ctx:Label("ProtoMapper", TextAlign.Left)

        Ctx:SpaceRowPush(new_rect(titleButtonsWidth + 70.0, 0.0, 25.0, 20.0))

        -- The app close button.
        if Ctx:ButtonLbl("X") then
            Win.Close()
        end

        Ctx:SpaceRowPush(new_rect(titleButtonsWidth + 35.0, 0.0, 25.0, 20.0))

        -- The app maximize/restore button.
        if Ctx:ButtonLbl("^") then
            Win.Toggle()
        end

        Ctx:SpaceRowPush(new_rect(titleButtonsWidth, 0.0, 25.0, 20.0))

        -- The app minimumize button.
        if Ctx:ButtonLbl("_") then
            Win.Iconify()
        end

        Ctx:SpaceRowEnd()

        Ctx:End()
    end


    -- The menu bar.
    if Ctx:Begin("Menubar", new_rect(0.0, 30.0, width, 30.0), 0) then
        Ctx:MenubarBegin()

        local default_len = string.len("Preferences")
        

        -- The menu bar.
        Ctx:StaticRow(20.0, 50, 15)

        if Ctx:MenuBeginLbl("File", TextAlign.Left, new_vec2(100, 4 * 30)) then
            Ctx:DynamicRow(20, 1)

            local len_n = string.len("New>")
            local len_o = string.len("Open>")
            local space_n = string.rep(" ", default_len - len_n)
            local space_o = string.rep(" ", default_len - len_o)

            Ctx:MenuItemLbl("New" .. space_n .. ">", TextAlign.Left)
            Ctx:MenuItemLbl("Open"  .. space_o .. ">", TextAlign.Left)
            Ctx:MenuItemLbl("Save", TextAlign.Left)
            Ctx:MenuItemLbl("Close", TextAlign.Left)
            
            Ctx:MenuEnd()
        end

        if Ctx:MenuBeginLbl("Edit", TextAlign.Left, new_vec2(120, 2 * 30)) then
            Ctx:StaticRow(20, 90, 1)

            Ctx:MenuItemLbl("Preferences", TextAlign.Left)
            Ctx:MenuItemLbl("Options", TextAlign.Left)
            
            Ctx:MenuEnd()
        end

        if Ctx:MenuBeginLbl("?", TextAlign.Left, new_vec2(100, 1 * 30)) then
            Ctx:StaticRow(20, 90, 1)

            Ctx:MenuItemLbl("About", TextAlign.Left)
            
            Ctx:MenuEnd()
        end

        Ctx:MenubarEnd()

        Ctx:End()
    end
    
end

-- The script must return the name of the function and the error message prefix you want to use to identify it.
return "CustomTitleBar", "[Lua: TitleBar] "
