# generate wm.inc and notify.inc

File.open 'wm.inc', 'w' do |f|
  %w[
  command close destroy size setfocus killfocus
  keydown keyup char syskeydown syskeyup syschar
  lbuttondown lbuttonup lbuttondblclk
  mbuttondown mbuttonup mbuttondblclk
  rbuttondown rbuttonup rbuttondblclk
  nclbuttondown nclbuttonup nclbuttondblclk
  ncmbuttondown ncmbuttonup ncmbuttondblclk
  ncrbuttondown ncrbuttonup ncrbuttondblclk
  mousewheel vscroll hscroll
  ].each do |s|
    f.puts %Q[case WM_#{s.upcase}: return rb_intern("on_#{s}");]
  end
end

File.open 'notify.inc', 'w' do |f|
  %w[
  en_change lbn_selchange
  ].each do |s|
    central = s[(s.index('_')+1)..-1]
    f.puts %Q[case #{s.upcase}: return rb_intern("on_#{central}");]
  end
end
