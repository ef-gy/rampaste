# RAMPaste

RAMPaste is a pastebin-clone that only uses RAM as its backing store. It has no
way of saving any information, so once the process is restarted it will lose all
of its state, forever.

Why would you want to do this? Because you can! And, let's be realistic, most of
the times you need to paste your clipboard to somewhere on the internets, you're
not going to keep recalling that more than a few times. So why put it on long
term storage when you can just keep it in RAM and have it recycle itself when
you're done?
