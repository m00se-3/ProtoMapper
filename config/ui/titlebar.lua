function CustomTitleBar(width)
    if Begin(Ctx, "ProtoMapper", new_rect(0.0, 0.0, width, 50.0), PanelFlag.NoScrollbar | PanelFlag.Border) ~= 0 then
        
        SpaceRowBegin(Ctx, Layout.Static, 20.0, 2)

        SpaceRowPush(Ctx, new_rect(0.0, 0.0, width - 100.0, 25.0))
        Label(Ctx, "ProtoMapper", TextAlign.Left)

        SpaceRowPush(Ctx, new_rect(width - 35.0, 0.0, 25.0, 20.0))
        if ButtonLbl(Ctx, "X") ~= 0 then
            RequestWindowToClose()
        end

        SpaceRowEnd(Ctx)

        SpaceRowBegin(Ctx, Layout.Static, 20.0, 1)

        SpaceRowPush(Ctx, new_rect(0.0, 0.0, 50.0, 20.0))
        if MenuBeginLbl(Ctx, "File", TextAlign.Left, new_vec2(100.0, 60.0)) ~= 0 then

            StaticRow(Ctx, 20.0, 90, 1)
            MenuItemLbl(Ctx, "New", TextAlign.Left)
            
            MenuEnd(Ctx)

        end

        SpaceRowEnd(Ctx)

    end

    End(Ctx)
end