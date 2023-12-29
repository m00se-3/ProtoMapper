--[[
    This function controls the program's title bar along with the menubar controls underneath.

    This function is not replaceable with a custom function.
]]
function CustomTitleBar(width)
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
            Ctx:RequestWindowToClose()
        end

        Ctx:SpaceRowPush(new_rect(titleButtonsWidth + 35.0, 0.0, 25.0, 20.0))

        -- The app maximize/restore button.
        if Ctx:ButtonLbl("^") then
            Ctx:RequestWindowToggle()
        end

        Ctx:SpaceRowPush(new_rect(titleButtonsWidth, 0.0, 25.0, 20.0))

        -- The app minimumize button.
        if Ctx:ButtonLbl("_") then
            Ctx:RequestWindowIconify()
        end

        Ctx:SpaceRowEnd()

        Ctx:End()
    end


    -- The menu bar.
    if Ctx:Begin("Menubar", new_rect(0.0, 30.0, width, 30.0), 0) then
        Ctx:MenubarBegin()

        -- The menu bar.
        Ctx:StaticRow(20.0, 50, 15)

        -- We use numerical loops here to guarantee that all menubar items will be iterated in the correct order.
        for index = 1, #UI.menubar do
            local menu = UI.menubar -- Create a local reference to the menubar table to help with readability.

            if Ctx:MenuBeginLbl(menu[index].name, TextAlign.Left, new_vec2(100, #menu[index].items * 30)) then
                Ctx:StaticRow(20, 90, 1)

                for ind = 1, #menu[index].items do
                    Ctx:MenuItemLbl(menu[index].items[ind].name, TextAlign.Left)
                end
                
                Ctx:MenuEnd()
            end
        end

        Ctx:MenubarEnd()

        Ctx:End()
    end
    
end