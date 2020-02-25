#ifndef _H_WEBPAGE_
#define _H_WEBPAGE_
// This file is generated by generate_webpage_header.py
String webpage = "<html>"
"<head>"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, minimum-scale=1, maximum-scale=1\">"
"    <style>"
"        * { padding: 0; margin: 0; }"
"        body { background-color: #0097C1; }"
"    </style>"
"</head>"
"<body>"
"    <div style=\"position:fixed;top: 50pt; text-align:center; width: 100%; color:white; font-size:300%; font-weight:bold; text-transform:uppercase; font-family:sans-serif\" id=\"value\">connected</div>"
"    <div style=\"position:fixed;top: 150pt; text-align:center; width: 100%; color:white; font-size:300%; font-weight:bold; text-transform:uppercase; font-family:sans-serif\" id=\"args\"></div>"
"    <form action=\"\" target=\"tif\" id=\"form\">"
"        <iframe src=\"javascript: false;\" name=\"tif\" style=\"display: none;\" id=\"tif\">"
"        </iframe>"
"        <input id=\"arg_x\" name=\"arg_x\" type=\"hidden\"/>"
"        <input id=\"arg_y\" name=\"arg_y\" type=\"hidden\"/>"
"    </form>"
"    <script>"
"        var offset = 50;"
"        document.body.style.height = document.body.clientHeight + offset + 'px';"
"        document.body.style.width = document.body.clientWidth + offset + 'px';"
"        document.getElementsByTagName(\"html\")[0].style.height = document.body.style.height + 'px';"
"        document.getElementsByTagName(\"html\")[0].style.width = document.body.style.width + 'px';"
"        var moveHomePosition = function() {"
"            document.body.scrollTop = offset / 2;"
"            document.body.scrollLeft = offset / 2;"
"        };"
"        setTimeout(moveHomePosition, 100);"
"        var command ='stop';"
"        var deadzone = 20;"
"        var xlimit = document.body.clientWidth / 6;"
"        var ylimit = document.body.clientHeight / 4;"
"        var xcenter = document.body.clientWidth / 2;"
"        var ycenter = document.body.clientHeight / 2;"
"        var esp_port = 'http://192.168.4.1:8080';"
"        var el_form = document.getElementById('form');"
"        var last_drive_time = new Date().getTime();"
"        var keep_drive_interval_id = 0;"
""
"        var keepDriveFunction = function() {"
"            el_form.action = esp_port + '/drive';"
"            el_form.submit();"
"            command = 'drive';"
"            last_drive_time = new Date().getTime();"
"        };"
""
"        var driveFunction = function(event) {"
""
"            var x = event.touches[0].clientX - xcenter;"
"            var y = event.touches[0].clientY - ycenter;"
""
"            if (Math.abs(x) >= deadzone || Math.abs(y) >= deadzone)"
"            {"
"                var arg_x = 0;"
"                var arg_y = 0;"
"                var url = \"/drive\";"
""
"                if (x >= deadzone)"
"                    arg_x = Math.floor((x - deadzone) * 100 / xlimit);"
"                else if (x <= deadzone)"
"                    arg_x = Math.floor((x + deadzone) * 100 / xlimit);"
"                else"
"                    arg_x = 0;"
""
"                if (y >= deadzone)"
"                    arg_y = Math.floor((y - deadzone) * 100 / ylimit);"
"                else if ( y <= deadzone)"
"                    arg_y = Math.floor((y + deadzone) * 100 / ylimit);"
"                else"
"                    arg_y = 0;"
""
"                if (arg_x < -100)"
"                    arg_x = -100;"
"                if (arg_x > 100)"
"                    arg_x = 100;"
"                if (arg_y < -100)"
"                    arg_y = -100;"
"                if (arg_y > 100)"
"                    arg_y = 100;"
""
"                document.getElementById('arg_x').value = arg_x;"
"                document.getElementById('arg_y').value = arg_y;"
""
"                el_form.action = esp_port + url;"
"                el_form.submit();"
"                command = 'drive';"
"                last_drive_time = new Date().getTime();"
"                document.getElementById('value').innerHTML = url.replace(\"/\",\"\");"
"                document.getElementById('args').innerHTML = \"X: \" + arg_x + \" Y: \" + arg_y;"
""
"                if (keep_drive_interval_id == 0) {"
"                    keep_drive_interval_id = setInterval(keepDriveFunction, 100);"
"                }"
"            }"
"        };"
""
"        var stopFunction = function(event) {"
"            el_form.action = esp_port + '/stop';"
"            el_form.submit();"
"            command = 'stop';"
"            if (keep_drive_interval_id != 0) {"
"                clearInterval(keep_drive_interval_id);"
"                keep_drive_interval_id = 0;"
"            }"
"            setTimeout(moveHomePosition, 50);"
"            document.getElementById('value').innerHTML = 'stop';"
"            document.getElementById('args').innerHTML = '';"
"        };"
""
"        document.body.ontouchstart = driveFunction;"
"        document.body.ontouchmove = driveFunction;"
"        document.body.ontouchend = stopFunction;"
"    </script>"
"</body>"
"</html>"
;
#endif //_H_WEBPAGE_
