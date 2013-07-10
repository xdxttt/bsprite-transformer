xml = require('LuaXml')
table = require ('table')
local xfile = xml.load("texture_Worship.xml")
local package = xfile:find("package")
-- if this substatement is found…

local plist_table = {}
if package ~= nil then

     plist_table['frames'] = {}
     plist_table['metadata'] = {}
     plist_table['metadata']['format'] ='2'
     plist_table['metadata']['realTextureFileName'] =  package.texturename..".png"
     plist_table['metadata']['size'] = '{'..package.width..','..package.height..'}'
     plist_table['metadata']['smartupdate'] = ''
     plist_table['metadata']['textureFileName'] = package.texturename..".png"

     for i=1,(#package) do
          local frame = {}


          frame['frame'] ='{{'..package.width*package[i].coordinateX..','..package.height*package[i].coordinateY..'},{'..package[i].widthinpixel..','..package[i].heightinpixel..'}}'
          frame['offset'] ='{0,0}'
          frame['rotated'] =package[i].flipped
          frame['sourceColorRect'] = '{'..package[i].widthinpixel..','..package[i].heightinpixel..'}'
          frame['sourceSize'] = '{'..package[i].widthinpixel..','..package[i].heightinpixel..'}'
          plist_table['frames'] [package[i].id..".png"] = frame

     end

end


function table2plist(a_table)
     if (a_table == nil) then
          return "";
     end
     t_str = ''
     for key,val in pairs(a_table) do
          if type(val) == "table" then

               t_str = t_str..'<key>'..key..'</key>\n'..'<dict>\t\n'..table2plist(val)..'\t\n</dict>\n'
          elseif type(val) == "number" then
               t_str = t_str..'<key>'..key..'</key>\n'..'<integer>'..val..'</integer>\n'
          else
               t_str = t_str..'<key>'..key..'</key>\n <string>'..val..'</string>\n'
          end
     end

     return t_str
end

function makehead(plist_str)
     local str ="<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
     str = str.."<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"
     str = str.."<plist version=\"1.0\"><dict>"
     str = str..plist_str
     str = str.."</dict></plist>"
     return str
end

local str = table2plist(plist_table)
     str = makehead(str)
print(str)

local file=io.open("texture_Worship.plist","w")
file:write(str)
io.close()


