function ax = axis_pct(pct)
% AXIS_PCT       Set reasonable axis limits.
%
% AXIS_PCT(pct) sets axis limits to extend pct% beyond limits of plotted 
% objects.  Default is 5%.
% Works for linear or log scale.
% Unfortunately, the axes won't change when new points are plotted.

% Written by Tom Minka

if nargin < 1
  pct = 0.05;
end
ax = [Inf -Inf Inf -Inf Inf -Inf];

% find bounding box of plotted objects
children = get(gca,'children');
for child = children'
  if strcmp(get(child,'type'),'text')
    xyz = get(child,'position');
    % need to determine bounding box of the text
    if strcmp(get(gca,'xscale'), 'log')
      xyz(1) = log10(xyz(1));
    end
    if strcmp(get(gca,'yscale'), 'log')
      xyz(2) = log10(xyz(2));
    end
    if strcmp(get(gca,'zscale'), 'log')
      xyz(3) = log10(xyz(3));
    end
    c([1 2]) = xyz(1);
    c([3 4]) = xyz(2);
    c([5 6]) = xyz(3);
  else
    x = get(child,'xdata');
    x = x(finite(x));
    if strcmp(get(gca,'xscale'), 'log')
      x = x(x > 0);
      x = log10(x);
    end
    if isempty(x)
      c([1 2]) = 0;
    else
      c([1 2]) = [min(x(:)) max(x(:))];
    end
    y = get(child,'ydata');
    y = y(finite(y));
    if strcmp(get(gca,'yscale'), 'log')
      y = y(y > 0);
      y = log10(y);
    end
    if isempty(y)
      c([3 4]) = 0;
    else
      c([3 4]) = [min(y(:)) max(y(:))];
    end
    try
      z = get(child,'zdata');
      z = z(finite(z));
      if isempty(z)
	c([5 6]) = 0;
      else
	if strcmp(get(gca,'zscale'), 'log')
	  z = z(z > 0);
	  z = log10(z);
	end
	c([5 6]) = [min(z(:)) max(z(:))];
      end
    end
  end
  ax([1 3 5]) = min(ax([1 3 5]), c([1 3 5]));
  ax([2 4 6]) = max(ax([2 4 6]), c([2 4 6]));
end
dx = ax(2)-ax(1);
if dx == 0
  dx = 1;
end
dy = ax(4)-ax(3);
if dy == 0
  dy = 1;
end
dz = ax(6)-ax(5);
if dz == 0
  dz = 1;
end
ax = ax + [-dx dx -dy dy -dz dz]*pct;
if strcmp(get(gca,'xscale'), 'log')
  ax([1 2]) = 10.^(ax([1 2]));
end
if strcmp(get(gca,'yscale'), 'log')
  ax([3 4]) = 10.^(ax([3 4]));
end
if strcmp(get(gca,'zscale'), 'log')
  ax([5 6]) = 10.^(ax([5 6]));
end
% clip for 2D
ax = ax(1:length(axis));
if ~isempty(children)
  axis(ax);
end
if nargout < 1
  clear ax
end
