
--------------------------------
-- @module MenuItemFont
-- @extend MenuItemLabel
-- @parent_module cc

--------------------------------
--  returns the name of the Font <br>
-- js getFontNameObj
-- @function [parent=#MenuItemFont] getFontNameObj 
-- @param self
-- @return string#string ret (return value: string)
        
--------------------------------
--  set the font name <br>
-- c++ can not overload static and non-static member functions with the same parameter types<br>
-- so change the name to setFontNameObj<br>
-- js setFontName
-- @function [parent=#MenuItemFont] setFontNameObj 
-- @param self
-- @param #string name
-- @return MenuItemFont#MenuItemFont self (return value: cc.MenuItemFont)
        
--------------------------------
--  initializes a menu item from a string with a target/selector 
-- @function [parent=#MenuItemFont] initWithString 
-- @param self
-- @param #string value
-- @param #function callback
-- @return bool#bool ret (return value: bool)
        
--------------------------------
--  get font size <br>
-- js getFontSize
-- @function [parent=#MenuItemFont] getFontSizeObj 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
--  set font size<br>
-- c++ can not overload static and non-static member functions with the same parameter types<br>
-- so change the name to setFontSizeObj<br>
-- js setFontSize
-- @function [parent=#MenuItemFont] setFontSizeObj 
-- @param self
-- @param #int size
-- @return MenuItemFont#MenuItemFont self (return value: cc.MenuItemFont)
        
--------------------------------
--  set the default font name 
-- @function [parent=#MenuItemFont] setFontName 
-- @param self
-- @param #string name
-- @return MenuItemFont#MenuItemFont self (return value: cc.MenuItemFont)
        
--------------------------------
--  get default font size 
-- @function [parent=#MenuItemFont] getFontSize 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
--  get the default font name 
-- @function [parent=#MenuItemFont] getFontName 
-- @param self
-- @return string#string ret (return value: string)
        
--------------------------------
--  set default font size 
-- @function [parent=#MenuItemFont] setFontSize 
-- @param self
-- @param #int size
-- @return MenuItemFont#MenuItemFont self (return value: cc.MenuItemFont)
        
--------------------------------
-- js ctor
-- @function [parent=#MenuItemFont] MenuItemFont 
-- @param self
-- @return MenuItemFont#MenuItemFont self (return value: cc.MenuItemFont)
        
return nil
