function CustomTitleBar(width)
    if Ctx:Begin("ProtoMapper", new_rect(0.0, 0.0, width, 50.0), PanelFlag.NoScrollbar | PanelFlag.Border) ~= 0 then
        
        Ctx:SpaceRowBegin(Layout.Static, 20.0, 2)

        Ctx:SpaceRowPush(new_rect(0.0, 0.0, width - 100.0, 25.0))
        Ctx:Label("ProtoMapper", TextAlign.Left)

        Ctx:SpaceRowPush(new_rect(width - 35.0, 0.0, 25.0, 20.0))
        if Ctx:ButtonLbl("X") ~= 0 then
            Ctx:RequestWindowToClose()
        end

        Ctx:SpaceRowEnd()

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