<?php

/**
 * @file
 * Korpa main theme function wrapper. 
 */

module_load_include('inc', 'korpa', 'korpa.korpa');

/**
 * Theme function implementation.
 */
function korpa_add_to_cart_button($vars) {
  $path = $vars['path'];
  $css_class = $vars['css_class'];
  $options = array(
    'attributes' => array(
      'class' => array($css_class)
    )
  );
  return '<div class="basic-cart-add-to-cart">' . l(t('Add to cart'), $path, $options) . '</div>';
}

/**
 * Theme function implementation.
 */
function korpa_price($vars) {
  $price = $vars['price'];
  $html = '<span class="basic-cart-price">' . $price . '</span>';
  return $html;
}

/**
 * Theme implementation for rendering the total price.
 * 
 * @todo find a way to pass $price through the variables array.
 */
function theme_korpa_cart_total_price($variables) {
  $price = korpa_get_total_price();
  $total = $price->total;
  
  // Building the HTML.
  $html  = '<div class="basic-cart-cart-total-price-contents row">';
  $html .= '  <div class="basic-cart-total-price cell">' . t('Ukupna cena') . ': <strong>' . $total . '</strong></div>';
  $html .= '</div>';
  
  return $html;
}

/**
 * Theme implementation for rendering the cart element.
 */
function theme_korpa_render_cart_element($variables) {
  // Element name and nid.
  $name = $variables['form']['#name'];
  $nid = (int) str_replace(array('cartcontents[', ']'), '', $name);
  if (empty($nid)) {
    return '';
  }
  
  // Getting the node for it's title and description.
  $node = korpa_get_cart($nid);
 
  // Price and currency.
  $unit_price = isset($node->korpa_unit_price) ? $node->korpa_unit_price : 0;
  
  // Prefix.
  $prefix  = '<div class="basic-cart-cart-contents row">';
  $prefix .= '  <div class="basic-cart-cart-node-title cell">' . l($node->title, 'node/' . $nid) . '<br />';
  $prefix .= '  </div>';
  $prefix .= '  <div class="cell basic-cart-cart-unit-price"><strong>' . $unit_price .' '.variable_get('korpa_currency'). '</strong></div>';
  $prefix .= '  <div class="cell basic-cart-cart-x">x</div>';
  $prefix .= '  <div class="basic-cart-cart-quantity cell">';
  $prefix .= '    <div class="cell">';
  // Suffix.
  $suffix  = '    </div>';
  $suffix .= '  </div>';
  $suffix .= '</div>';
  
  // Rendering the element as textfield.
  $quantity = theme('textfield', $variables['form']);
  // Full view return.
  return $prefix . $quantity . $suffix;
}

