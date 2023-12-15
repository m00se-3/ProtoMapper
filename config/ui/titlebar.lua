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
    
    if Ctx:Begin("ProtoMapper", new_rect(0.0, 0.0, width, 50.0), PanelFlag.NoScrollbar | PanelFlag.Border) ~= 0 then
        
        Ctx:SpaceRowBegin(Layout.Static, 20.0, 4)

        -- The app title.
        Ctx:SpaceRowPush(new_rect(0.0, 0.0, titleLabelWidth, 25.0))
        Ctx:Label("ProtoMapper", TextAlign.Left)

        Ctx:SpaceRowPush(new_rect(titleButtonsWidth + 70.0, 0.0, 25.0, 20.0))

        -- The app close button.
        if Ctx:ButtonLbl("X") ~= 0 then
            Ctx:RequestWindowToClose()
        end

        Ctx:SpaceRowPush(new_rect(titleButtonsWidth + 35.0, 0.0, 25.0, 20.0))

        -- The app maximize/restore button.
        if Ctx:ButtonLbl("^") ~= 0 then
            Ctx:RequestWindowToggle()
        end

        Ctx:SpaceRowPush(new_rect(titleButtonsWidth, 0.0, 25.0, 20.0))

        -- The app minimumize button.
        if Ctx:ButtonLbl("_") ~= 0 then
            Ctx:RequestWindowIconify()
        end

        Ctx:SpaceRowEnd()

        -- The menu bar.
        Ctx:SpaceRowBegin(Layout.Static, 20.0, 1)

        Ctx:SpaceRowPush(new_rect(0.0, 0.0, 50.0, 20.0))
        if Ctx:MenuBeginLbl("File", TextAlign.Left, new_vec2(100.0, 60.0)) ~= 0 then
            Ctx:StaticRow(20.0, 90, 1)
            Ctx:MenuItemLbl("New", TextAlign.Left)
            
            Ctx:MenuEnd()
        end

        Ctx:SpaceRowEnd()

    end

    Ctx:End()
end